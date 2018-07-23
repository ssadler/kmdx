#include <err.h>
#include "ConsensusState.h"


void ConsensusState::tryAddVote(Vote vote, HexBytes peerID) {
    try {
        bool added = addVote(vote, peerID);
    } catch (ErrVoteHeightMismatch heightMismatch) {
        // If the vote height is off, we'll just ignore it,
        // But if it's a conflicting sig, add it to the cs.evpool.
        // If it's otherwise invalid, punish peer.
        throw heightMismatch;
    } catch (ErrVoteConflictingVotes conflictingVotes) {
        if (vote.getValidatorAddress() == privValidator.getAddress()) {
            logError("Found conflicting vote from ourselves. Did you unsafe_reset a validator?", "height",
                     vote.getHeight(), "roundNumber", vote.getRoundNumber(), "type", vote.getType());
            throw conflictingVotes;
        }
    } catch (Error e) {
        // Probably an invalid signature / Bad peer.
        // Seems this can also err sometimes with "Unexpected stepType" - perhaps not from a bad peer ?
        logError("Error attempting to add vote", "err", err);
        throw new ErrAddingVote(e.getDescription());
    }
}

//-----------------------------------------------------------------------------
/** returns true if it was added */
bool ConsensusState::addVote(Vote vote, HexBytes peerID) {
    logDebug("addVote", "voteHeight", vote.getHeight(), "voteType", voteTypeToString(vote.getType()), "valIndex",
             vote.getValidatorIndex(), "csHeight", roundState.height);
    switch (vote.getHeight() - roundState.height) {
        case 1: //TODO WTF
            // A precommit for the previous height?
            // These come in while we wait timeoutCommit
            if (!(stepType == RoundStepNewHeight && vote.getType() == VoteTypePrecommit)) {
                // TODO: give the reason ..
                // fmt.Errorf("tryAddVote: Wrong height, not a LastCommit straggler commit.")
                throw new ErrVoteHeightMismatch();
            }

            //throws exception
            roundState.lastCommit->addVote(vote);
            logInfo("Added to lastPrecommits: %v", roundState.lastCommit->toStringShort());
            eventBus->publishEventVote(EventDataVote(vote));
            eventSwitch.fireEvent(EventVote(), vote);

            // if we can skip timeoutCommit and have all the votes now,
            if (consensusConfig.isSkipTimeoutCommit() && roundState.lastCommit->hasAllVotes()) {
                // go straight to new roundNumber (skip timeout commit)
                // cs.scheduleTimeout(time.Duration(0), cs.Height, 0, cstypes.RoundStepNewHeight)
                enterNewRound(roundState.height, 0);
            }
            return true;
        case 0:
            eventBus->publishEventVote(EventDataVote(vote));
            eventSwitch.fireEvent(EventVote(), vote);
            addVoteForCurrentRound(vote);
            return true;
        default:
            // Height mismatch is ignored.
            // Not necessarily a bad peer, but not favourable behaviour.
            if (vote.getHeight() != roundState.height) {
                logInfo("Vote ignored and not added", "voteHeight", vote.getHeight(), "csHeight", roundState.height,
                        "err", err);
                throw new ErrVoteHeightMismatch();
            }
    }


}


//my own method
/** throws ExceptionUnexpectedVoteType */
void ConsensusState::addVoteForCurrentRound(Vote vote) {
    switch (vote.getType()) {
        case VoteTypePrevote:
            VoteSet prevotes = roundState.votes->getPrevotes(vote.getRoundNumber());
            logInfo("Added to prevote", "vote", vote, "prevotes", prevotes.toStringShort());
            BlockID blockId;
            try {
                // If +2/3 prevotes for a block or nil for *any* roundNumber:
                if (prevotes.twoThirdMajority(blockId)) {
                    // There was a polka!
                    unlockBlock(vote, blockId);
                }
                tryUpdateValidRoundAndBlock(vote, blockId);
                // If +2/3 prevotes for *anything* for this or future roundNumber:
                if (roundState.roundNumber <= vote.getRoundNumber() && prevotes.hasTwoThirdsAny()) {
                    // Round-skip over to PrevoteWait or goto Precommit.
                    enterNewRound(roundState.height, vote.getRoundNumber()); // if the vote is ahead of us
                    if (prevotes.hasTwoThirdMajority()) {
                        enterPrecommit(roundState.height, vote.getRoundNumber());
                    } else {
                        enterPrevote(roundState.height, vote.getRoundNumber()); // if the vote is ahead of us
                        enterPrevoteWait(roundState.height, vote.getRoundNumber());
                    }
                } else if (
                        proposal != NULL
                        && 0 <= proposal->getPolRound()
                        && proposal->getPolRound() == vote.getRoundNumber()
                        ) {
                    // If the proposal is now complete, enter prevote of cs.Round.
                    if (isProposalComplete()) {
                        enterPrevote(roundState.height, roundState.roundNumber);
                    }
                }
            } catch (Exception e) {
                //TODO
            }
            break;
        case VoteTypePrecommit:
            VoteSet precommits = roundState.votes->getPrecommits(vote.getRoundNumber());
            logInfo("Added to precommit", "vote", vote, "precommits", precommits.toStringShort());
            BlockID blockID;
            if (precommits.twoThirdMajority(blockID)) {
                if (blockID.hash().getBites().size() == 0) {
                    enterNewRound(roundState.height, vote.getRoundNumber() + 1);
                } else {
                    enterNewRound(roundState.height, vote.getRoundNumber());
                    enterPrecommit(roundState.height, vote.getRoundNumber());
                    enterCommit(roundState.height, vote.getRoundNumber());

                    if (consensusConfig.isSkipTimeoutCommit() && precommits.hasAll()) {
                        // if we have all the votes now,
                        // go straight to new round (skip timeout commit)
                        // cs.scheduleTimeout(time.Duration(0), cs.Height, 0, cstypes.RoundStepNewHeight)
                        enterNewRound(roundState.height, 0);
                    }
                }
            } else if (roundState.roundNumber <= vote.getRoundNumber() && precommits.hasTwoThirdsAny()) {
                enterNewRound(roundState.height, vote.getRoundNumber());
                enterPrecommit(roundState.height, vote.getRoundNumber());
                enterPrecommitWait(roundState.height, vote.getRoundNumber());
            }
            break;
        default:
            throw new ExceptionUnexpectedVoteType("Unexpected vote type %X",
                                                  vote.getType()); // go-wire should prevent this.
    }
}

void ConsensusState::tryUpdateValidRoundAndBlock(const Vote &vote, BlockID &blockId) {// Update Valid* if we can.
    // NOTE: our proposal block may be nil or not what received a polka..
    // TODO: we may want to still update the ValidBlock and obtain it via gossipping
    if (!blockId.isEmpty() //TODO replace with isZero()
        && roundState.validRoundNumber < vote.getRoundNumber()
        && vote.getRoundNumber() <= roundState.roundNumber
        && roundState.proposalBlock->hashesTo(blockId.hash())
            ) {
        logInfo("Updating ValidBlock because of POL.", "validRound", roundState.validRoundNumber, "POLRound",
                vote.getRoundNumber());
        roundState.setValidRoundNumber(vote.getRoundNumber());
        roundState.setValidBlock(roundState.proposalBlock);
        //roundState.setValidBlockParts(roundState.proposalBlockParts);
    }
}

void ConsensusState::unlockBlock(const Vote &vote,
                                 BlockID &blockId) {// If we're locked but this is a recent polka, unlock.
    // If it matches our ProposalBlock, update the ValidBlock

    // Unlock if `cs.LockedRound < vote.Round <= cs.Round`
    // NOTE: If vote.Round > cs.Round, we'll deal with it when we get to vote.Round
    if (roundState.lockedBlock != NULL
        && roundState.lockedRoundNumber < vote.getRoundNumber()
        && vote.getRoundNumber() <= roundState.roundNumber
        && !roundState.lockedBlock->hashesTo(blockId.hash())
            ) {
        logInfo("Unlocking because of POL.", "lockedRound", roundState.lockedRoundNumber, "POLRound",
                vote.getRoundNumber());
        roundState.setLockedRoundNumber(0);
        roundState.setLockedBlock(NULL);
        //roundState.setLockedBlockParts(NULL);
        eventBus->publishEventUnlock(roundState.roundStateEvent());
    }
}


void ConsensusState::doPrevote(int64_t height, int _roundNumber) {
// If a block is locked, prevote that.
    if (roundState.lockedBlock != NULL) {
        logInfo("enterPrevote: Block was locked");
        //TODO header signAddVote(types.VoteTypePrevote, cs.LockedBlock.Hash(), cs.LockedBlockParts.Header())
        return;
    }

    // If ProposalBlock is nil, prevote nil.
    if (roundState.proposalBlock == NULL) {
        logInfo("enterPrevote: ProposalBlock is nil");
        //todo header signAddVote(types.VoteTypePrevote, nil, types.PartSetHeader{})
        return;
    }

    /*// Validate proposal block
    try {
        blockExec.validateBlock(state, roundState.proposalBlock);
    } catch (Exception e) {
                   // ProposalBlock is invalid, prevote nil.
                logError("enterPrevote: ProposalBlock is invalid", "err", e);
                //signAddVote(types.VoteTypePrevote, nil, types.PartSetHeader{})
                return;
        }*/

    // Prevote cs.ProposalBlock
    // NOTE: the proposal signature is validated when it is received,
    // and the proposal block parts are validated as they are received (against the merkle hash in the proposal)
    logInfo("enterPrevote: ProposalBlock is valid"); //TODO change message if theres no validation
    //TODO signAddVote(VoteTypePrevote, roundState.proposalBlock->hash(), roundState.proposalBlockParts.Header());
}
