#include "ConsensusState.h"
#include "../helpers/Bridge.h"


void ConsensusState::tryAddVote(Vote vote, HexBytes peerID) {
    try {
        //bool added =
        addVote(vote, peerID);
    } catch (ErrVoteHeightMismatch &heightMismatch) {
        // If the vote height is off, we'll just ignore it,
        // But if it's a conflicting sig, add it to the cs.evpool.
        // If it's otherwise invalid, punish peer.
        throw heightMismatch;
    } catch (ErrVoteConflictingVotes &conflictingVotes) {
        if (vote.getValidatorAddress() == privValidator.get()->getAddress()) {
            /*//clog(dev::VerbosityError, channelTm) << "Found conflicting vote from ourselves. Did you unsafe_reset a validator?"<< "height"<<
                     vote.getHeight()<< "roundNumber"<< vote.getRoundNumber()<< "type", vote.getType();*/
            throw conflictingVotes;
        }
    } catch (Error &e) {
        // Probably an invalid signature / Bad peer.
        // Seems this can also err sometimes with "Unexpected stepType" - perhaps not from a bad peer ?
        //clog(dev::VerbosityError, channelTm) << "Error attempting to add vote"<< "err"<< err;
        throw ErrAddingVote(e.getDescription());
    }
}

//-----------------------------------------------------------------------------
/** returns true if it was added */
bool ConsensusState::addVote(Vote vote, HexBytes peerID) {
    /*//clog(dev::VerbosityDebug, channelTm) << "addVote"<< "voteHeight"<< vote.getHeight()<< "voteType"<< voteTypeToString(vote.getType())<< "valIndex"<<
             vote.getValidatorIndex()<< "csHeight"<< roundState.height;*/
    switch (roundState.height - vote.getHeight()) {
        case 1:
            if (stepType == RoundStepNewHeight && vote.getType() == VoteTypePrecommit) {
                // A precommit for the previous height?
                // These come in while we wait timeoutCommit
                handleStragglerCommit(vote); //throws PanicNullVoteSet

            } else {
                // TODO: give the reason ..
                // fmt.Errorf("tryAddVote: Wrong height, not a LastCommit straggler commit.")
                throw ErrVoteHeightMismatch();
            }
            return true;
        case 0:
            roundState.height = roundState.votes.addVote(vote, peerID);
            eventBus->publishEventVote(EventDataVote(vote));
            eventSwitch.fireEvent(EventVote(), vote);
            addVoteForCurrentRound(vote);
            return true;
        default:
            // Height mismatch is ignored.
            // Not necessarily a bad peer, but not favourable behaviour.
            if (vote.getHeight() != roundState.height) {
                /* //clog(dev::VerbosityInfo, channelTm) << "Vote ignored and not added", "voteHeight"<< vote.getHeight()<< "csHeight"<< roundState.height<<
                         "err"<< err;*/
                throw ErrVoteHeightMismatch();
            }
    }
    return true;

}

void ConsensusState::handleStragglerCommit(const Vote &vote) {
    roundState.lastCommit->addVote(vote);
    //clog(dev::VerbosityInfo, channelTm) << "Added to lastPrecommits: %v" << roundState.lastCommit->toStringShort();
    eventBus->publishEventVote(EventDataVote(vote));
    eventSwitch.fireEvent(EventVote(), vote);

    // if we can skip timeoutCommit and have all the votes now,
    if (consensusConfig.isSkipTimeoutCommit() && roundState.lastCommit->hasAllVotes()) {
        // go straight to new roundNumber (skip timeout commit)
        // cs.scheduleTimeout(time.Duration(0), cs.Height, 0, cstypes.RoundStepNewHeight)
        enterNewRound(roundState.height, 0);
    }
}


//my own method

void ConsensusState::addVoteForCurrentRound(Vote vote) {
    VoteSet prevotes;
    VoteSet precommits;
    BlockID blockId;
    switch (vote.getType()) {

        case VoteTypePrevote:
            prevotes = roundState.votes.getPrevotes(vote.getRoundNumber());
            //clog(dev::VerbosityInfo, channelTm) << "Added to prevote"<< "vote"<< vote<< "prevotes"<< prevotes.toStringShort();

            // If +2/3 prevotes for a block or nil for *any* roundNumber:
            if (prevotes.twoThirdMajority(blockId)) {
                // There was a polka!
                unlockBlock(vote, blockId);
                tryUpdateValidRoundAndBlock(vote, blockId);
            }

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
                    proposal != nullptr
                    && 0 <= proposal->getPolRound()
                    && proposal->getPolRound() == vote.getRoundNumber()
                    ) {
                // If the proposal is now complete, enter prevote of cs.Round.
                if (isProposalComplete()) {
                    enterPrevote(roundState.height, roundState.roundNumber);
                }
            }

            break;
        case VoteTypePrecommit:
            precommits = *roundState.votes.getPrecommits(vote.getRoundNumber()).get();
            //clog(dev::VerbosityInfo, channelTm) << "Added to precommit"<< "vote"<< vote<< "precommits"<< precommits.toStringShort();

            if (precommits.twoThirdMajority(blockId)) {
                if (blockId.getHash().getBites().empty()) {
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
            throw ErrorUnexpectedVoteType(vote.getType()); // go-wire should prevent this. */
    }
}

/* Update Valid* if we can. */
void ConsensusState::tryUpdateValidRoundAndBlock(const Vote &vote, BlockID &blockId) {
    // NOTE: our proposal block may be nil or not what received a polka..
    // TODO: we may want to still update the ValidBlock and obtain it via gossipping
    if (!blockId.isEmpty()
        && roundState.validRoundNumber < vote.getRoundNumber()
        && vote.getRoundNumber() <= roundState.roundNumber
        && roundState.proposalBlock->hashesTo(blockId.getHash())
            ) {
        //clog(dev::VerbosityInfo, channelTm) << "Updating ValidBlock because of POL."<<  "validRound"<<  roundState.validRoundNumber<<  "POLRound"<< 
        vote.getRoundNumber();
        roundState.setValidRoundNumber(vote.getRoundNumber());
        roundState.setValidBlock(roundState.proposalBlock);
        //roundState.setValidBlockParts(roundState.proposalBlockParts);
    }
}

void ConsensusState::unlockBlock(const Vote &vote,
                                 BlockID &blockId) {
    // If we're locked but this is a recent polka, unlock.
    // If it matches our ProposalBlock, update the ValidBlock

    // Unlock if `cs.LockedRound < vote.Round <= cs.Round`
    // NOTE: If vote.Round > cs.Round, we'll deal with it when we get to vote.Round
    if (roundState.lockedBlock != nullptr
        && roundState.lockedRoundNumber < vote.getRoundNumber()
        && vote.getRoundNumber() <= roundState.roundNumber
        && !roundState.lockedBlock->hashesTo(blockId.getHash())
            ) {
        //clog(dev::VerbosityInfo, channelTm) << "Unlocking because of POL."<<  "lockedRound"<<  roundState.lockedRoundNumber<<  "POLRound"<< 
        vote.getRoundNumber();
        roundState.setLockedRoundNumber(0);
        roundState.setLockedBlock(nullptr);
        //roundState.setLockedBlockParts(NULL);
        eventBus->publishEventUnlock(roundState.roundStateEvent());
    }
}


void ConsensusState::doPrevote(int64_t height, int _roundNumber) {

    //TODO unused parameters
    if (height == _roundNumber) return;
// If a block is locked, prevote that.
    if (roundState.lockedBlock != nullptr) {
        //clog(dev::VerbosityInfo, channelTm) << "enterPrevote: Block was locked";
        //TODO header signAddVote(types.VoteTypePrevote, cs.LockedBlock.Hash(), cs.LockedBlockParts.Header())
        return;
    }

    // If ProposalBlock is nil, prevote nil.
    if (roundState.proposalBlock == nullptr) {
        //clog(dev::VerbosityInfo, channelTm) << "enterPrevote: ProposalBlock is nil";
        signAddVote(VoteTypePrevote);
        return;
    }

    /*// TODO Validate proposal block
    try {
        blockExec.validateBlock(state, roundState.proposalBlock);
    } catch (Exception e) {
                   // ProposalBlock is invalid, prevote nil.
                //clog(dev::VerbosityError, channelTm) << "enterPrevote: ProposalBlock is invalid", "err", e);
                //signAddVote(types.VoteTypePrevote, nil, types.PartSetHeader{})
                return;
        }*/

    // Prevote cs.ProposalBlock
    // NOTE: the proposal signature is validated when it is received,
    // and the proposal block parts are validated as they are received (against the merkle hash in the proposal)
    //clog(dev::VerbosityInfo, channelTm) << "enterPrevote: ProposalBlock is valid";
    signAddVote(VoteTypePrevote, roundState.proposalBlock->getBlockHash());
}


/** Enter: any +2/3 precommits for next round.*/
void ConsensusState::enterPrecommitWait(int64_t height, int round) {
    if (roundState.height != height || round < roundState.roundNumber ||
        (roundState.roundNumber == round && RoundStepPrecommitWait <= roundState.stepType)) {
        /*//clog(dev::VerbosityDebug, channelTm) << "enterPrecommitWait(%v/%v): Invalid args. Current step: %v/%v/%v"<< height<< round<< roundState.height<<
                 roundState.roundNumber<< roundState.stepType;*/
        return;
    }
    if (!roundState.votes.getPrecommits(round).get()->hasTwoThirdsAny()) {
        throw PanicEnterRound("enterPrecommitWait(" + to_string(height) + " " + to_string(round) +
                              ") but Precommits does not have any +2/3 votes");
    }
    /*//clog(dev::VerbosityInfo, channelTm) << "enterPrecommitWait(%v/%v). Current: %v/%v/%v"<< height<< round<< roundState.height<< roundState.roundNumber<<
            roundState.stepType;*/
    auto guard = finally([&]() {
        roundState.updateRoundStep(roundState.roundNumber, RoundStepPrecommitWait);
        newStep();
    });

    // Wait for some more precommits; enterNewRound
    scheduleTimeout(consensusConfig.getTimeoutPrecommit(round), height, round, RoundStepPrecommitWait);

}

void ConsensusState::tryFinalizeCommit(int64_t height) {
    //logger := cs.Logger.With("height", height)

    if (roundState.height != height) {
        throw PanicEnterRound(
                "tryFinalizeCommit() cs.Height: " + to_string(roundState.height) + " vs height: " + to_string(height));
    }
    BlockID blockID;
    bool ok = roundState.votes.getPrecommits(roundState.commitRoundNumber).get()->twoThirdMajority(blockID);
    if (!ok || blockID.getHash().getBites().empty()) {
        //clog(dev::VerbosityError, channelTm) << "Attempt to finalize failed. There was no +2/3 majority, or +2/3 was for <nil>.";
        return;
    }
    if (!roundState.proposalBlock->hashesTo(blockID.getHash())) {
        // TODO: this happens every time if we're not a validator (ugly logs)
        // TODO: ^^ wait, why does it matter that we're a validator?
/*        //clog(dev::VerbosityInfo, channelTm) << "Attempt to finalize failed. We don't have the commit block."<< "proposal-block"<<
                roundState.proposalBlock->getHash().toString()<< "commit-block"<< blockID.getHash().toString();*/
        return;
    }

    //	go
    finalizeCommit(height);
}

/* Increment height and goto cstypes.RoundStepNewHeight */
void ConsensusState::finalizeCommit(int64_t _height) {
    if (roundState.height != _height || roundState.stepType != RoundStepCommit) {
        /* //clog(dev::VerbosityDebug, channelTm) << "finalizeCommit(%v): Invalid args. Current step: %v/%v/%v"<< _height<< roundState.height<<
                  roundState.roundNumber<< roundState.stepType;*/
        return;
    }
    BlockID blockID;
    bool ok = roundState.votes.getPrecommits(roundState.commitRoundNumber).get()->twoThirdMajority(blockID);
    auto block = roundState.proposalBlock;
    //blockParts = roundState.proposalBlockParts;
    if (!ok) {
        throw PanicNoPolka("Cannot finalizeCommit, ");
    }
    /*if (!blockParts.HasHeader(blockID.PartsHeader)) {
        throw Panic("Expected ProposalBlockParts header to be commit header");
    }*/
    if (!block->hashesTo(blockID.getHash())) {
        throw PanicInvalidHash("Cannot finalizeCommit, ProposalBlock does not hash to commit hash");
    }
    /*if (err = roundState.blockExec.ValidateBlock(roundState.state, block); err != nil) {
        throw PanicConsensus("+2/3 committed an invalid block: %v", err);
    }*/

    /* //clog(dev::VerbosityInfo, channelTm) << "Finalizing commit of block with height "<< to_string(block->getHeight())<< " hash "<<
             block->getBlockHash().toString()<< " root ", block->getAppHash().toString();*/
    //clog(dev::VerbosityInfo, channelTm) << "%v" << block;

    //todo WTF fail.Fail(); // XXX
    //TODO commit block
    /* // Save to blockStore.
     if (roundState.blockStore._height() < block._height) {
         // NOTE: the seenCommit is local justifi(cation to commit this block,
         // but may differ from the LastCommit included in the next block
         precommits = roundState.votes.getPrecommits(roundState.commitRound);
         seenCommit = precommits.MakeCommit();
         roundState.blockStore.SaveBlock(block, blockParts, seenCommit);
     } else {
         // Happens during replay if (we already saved the block but didn't commit
         //clog(dev::VerbosityInfo, channelTm) << "Calling finalizeCommit on already stored block", "_height", block._height);
     }

    // fail.Fail(); // XXX */

    // Write EndHeightMessage{} for this _height, implying that the blockstore
    // has saved the block.
    //
    // If we crash before writing this EndHeightMessage{}, we will recover by
    // running ApplyBlock during the ABCI handshake when we restart.  If we
    // didn't save the block to the blockstore before writing
    // EndHeightMessage{}, we'd have to change WAL replay -- currently it
    // complains about replaying for heights where an #ENDHEIGHT entry already
    // exists.
    //
    // Either way, the ConsensusState should not be resumed until we
    // successfully call ApplyBlock (ie. later here, or in Handshake after
    // restart).
    //TODO write ahead log roundState.wal.WriteSync(EndHeightMessage{_height}); // NOTE: fsync
/*
    //fail.Fail(); // XXX

    // Create a copy of the state for staging and an event cache for txs.
    stateCopy = roundState.state.Copy();

    // Execute and commit the block, update and save the state, and update the mempool.
    // NOTE The block.AppHash wont reflect these txs until the next block.
    var err error
    stateCopy, err = roundState.blockExec.ApplyBlock(stateCopy, types.BlockID{block.Hash(), blockParts.Header()}, block)
    if (err != nil) {
        //clog(dev::VerbosityError, channelTm) << "Error on ApplyBlock. Did the application crash? Please restart tendermint", "err", err);
        err = cmn.Kill();
        if (err != nil) {
            //clog(dev::VerbosityError, channelTm) << "Failed to kill this process - please do so manually", "err", err);
        }
        return;
    }

    fail.Fail(); // XXX
*/
    // NewHeightStep!
    updateToState(state);

    //fail.Fail(); // XXX

    // roundState.startTime is already set.
    // Schedule Round0 to start soon.
    scheduleRound0(roundState);

    // By here,
    // * roundState.height has been increment to _height+1
    // * roundState.step is now RoundStepNewHeight
    // * roundState.startTime is set to when we will start round0.
}


void ConsensusState::scheduleRound0(RoundState roundState) {
//cs.Logger.Info("scheduleRound0", "now", time.Now(), "startTime", cs.StartTime)

    scheduleTimeout(roundState.startTime - boost::posix_time::second_clock::local_time(), roundState.height, 0,
                    RoundStepNewHeight);
}


void ConsensusState::clearProposal() {
    this->proposal = nullptr;

}

bool ConsensusState::isValidator() {
    return (privValidator != nullptr && roundState.validators.hasAddress(privValidator->getAddress()));
}

bool ConsensusState::isProposer() {
    return roundState.validators.getProposer().getAddress() == privValidator.get()->getAddress();
}


void ConsensusState::decideProposal(int64_t height, int round) {
    shared_ptr<Block> block;

    // Decide on block
    if (roundState.lockedBlock != nullptr) {
        block = roundState.lockedBlock;
    } else if (roundState.validBlock != nullptr) {
        block = roundState.validBlock;
    } else {
        block = Bridge().getBlockToPropose();
    }

    // Make proposal
    BlockID polBlockID(-1);
    int polRound = roundState.votes.polInfo(polBlockID);
    Proposal proposal(height, round, polRound, polBlockID);
    try {
        privValidator.get()->signProposal(state.getChainID(), proposal);
    } catch (exception &e) {
        if (!replayMode) {
            //clog(dev::VerbosityError, channelTm) << "enterPropose: Error signing proposal", "height", height, "round", round, "err", err);
        }
        return;
    }

    // Set fields
    /*  fields set by setProposal and addBlockPart
    roundStep.Proposal = proposal
    roundStep.ProposalBlock = block
    roundStep.ProposalBlockParts = blockParts
    */

    // send proposal and block parts on internal msg queue
    sendInternalMessage(ProposalMessage(proposal));
    sendInternalMessage(BlockMessage(PeerID(std::vector<uint8_t>()), polBlockID, roundState.height, roundState.roundNumber, block));

    //clog(dev::VerbosityInfo, channelTm) << "Signed proposal", "height", height, "round", round, "proposal", proposal);
    //clog(dev::VerbosityDebug, channelTm) << "Signed proposal block: %v", block);
}

