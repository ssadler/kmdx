#include "ConsensusState.h"


void ConsensusState::tryAddVote(Vote &vote, AddressTm addresstm) {
    try {
        //bool added =
        addVote(vote, addresstm);
    } catch (ErrVoteHeightMismatch &heightMismatch) {
        // If the vote height is off, we'll just ignore it,
        // But if it's a conflicting sig, add it to the cs.evpool.
        // If it's otherwise invalid, punish peer.
        throw heightMismatch;
    } catch (ErrVoteConflictingVotes &conflictingVotes) {
        if (vote.getValidatorAddress() == privValidator->getAddress()) {
            clog(dev::VerbosityError, Logger::channelTm)
                << "Found conflicting vote from ourselves. Did you unsafe_reset a validator?" << "height"
                << vote.getHeight() << "roundNumber" << vote.getRoundNumber() << "type", vote.getType();
            throw conflictingVotes;
        }
    } catch (Error &e) {
        // Probably an invalid signature / Bad peer.
        // Seems this can also err sometimes with "Unexpected stepType" - perhaps not from a bad peer ?
        clog(dev::VerbosityError, Logger::channelTm) << "Error attempting to add vote" << "err" << err;
        throw ErrAddingVote(e.getDescription(), __FILE__, __LINE__);
    }
}

//-----------------------------------------------------------------------------
/** returns true if it was added */
bool ConsensusState::addVote(Vote &vote, P2PID pid) {
    clog(dev::VerbosityDebug, Logger::channelTm) << "addVote" << "voteHeight" << vote.getHeight() << "voteType"
                                                 << Vote::voteTypeToString(vote.getType()) << "valIndex"
                                                 << vote.getValidatorIndex()
                                                 << "csHeight" << roundState.height;
    switch (roundState.height - vote.getHeight()) {
        case 1:
            if (stepType == RoundStepNewHeight && vote.getType() == VoteTypePrecommit) {
                // A precommit for the previous height?
                // These come in while we wait timeoutCommit
                handleStragglerCommit(vote); //throws PanicNullVoteSet

            } else {
                // TODO: give the reason ..
                // fmt.Errorf("tryAddVote: Wrong height, not a LastCommit straggler commit.")
                throw ErrVoteHeightMismatch(__FILE__, __LINE__);
            }
            return true;
        case 0:
            if (!roundState.votes.addVote(vote, pid).isAdded())
                return false;
            eventBus.publishEventVote(EventDataVote(vote));
            eventSwitch.fireEvent(EventVote(), vote);
            addVoteForCurrentRound(vote);
            return true;
        default:
            // Height mismatch is ignored.
            // Not necessarily a bad peer, but not favourable behaviour.
            if (vote.getHeight() != roundState.height) {
                clog(dev::VerbosityInfo, Logger::channelTm) << "Vote ignored and not added" << "voteHeight"
                                                            << vote.getHeight()
                                                            << "csHeight"
                                                            << roundState.height;//<<                         "err" << err;
                throw ErrVoteHeightMismatch(__FILE__, __LINE__);
            }
    }
    return true;

}

void ConsensusState::handleStragglerCommit(Vote &vote) {
    boost::optional<Vote> conflicting;
    roundState.lastCommit->addVote(vote, conflicting);
    clog(dev::VerbosityInfo, Logger::channelTm) << "Added to lastPrecommits: %v"
                                                << roundState.lastCommit->toStringShort();
    eventBus.publishEventVote(EventDataVote(vote));
    eventSwitch.fireEvent(EventVote(), vote);

    // if we can skip timeoutCommit and have all the votes now,
    if (consensusConfig.isSkipTimeoutCommit() && roundState.lastCommit->hasAll()) {
        // go straight to new roundNumber (skip timeout commit)
        // cs.scheduleTimeout(time.Duration(0), cs.Height, 0, cstypes.RoundStepNewHeight)
        enterNewRound(roundState.height, 0);
    }
}


//my own method

void ConsensusState::addVoteForCurrentRound(Vote &vote) {
    VoteSet *prevotes;
    VoteSet *precommits;
    boost::optional<BlockID> blockId;
    switch (vote.getType()) {

        case VoteTypePrevote:
            prevotes = roundState.votes.getPrevotes(vote.getRoundNumber());
            if (!prevotes) throw PanicNullVoteSet(__FILE__, __LINE__);
            clog(dev::VerbosityInfo, Logger::channelTm) << "Added to prevote" << "vote" << vote.toString() << "prevotes"
                                                        << prevotes->toStringShort();
            // If +2/3 prevotes for a block or nil for *any* roundNumber:
            blockId = prevotes->twoThirdMajority();

            if (blockId.is_initialized()) {
                // There was a polka!
                blockId = prevotes->twoThirdMajority();
                unlockBlock(vote, blockId.get());
                tryUpdateValidRoundAndBlock(vote, blockId.get());
            }

            // If +2/3 prevotes for *anything* for this or future roundNumber:
            if (roundState.roundNumber <= vote.getRoundNumber() && prevotes->hasTwoThirdsAny()) {
                // Round-skip over to PrevoteWait or goto Precommit.
                enterNewRound(roundState.height, vote.getRoundNumber()); // if the vote is ahead of us
                if (prevotes->hasTwoThirdMajority()) {
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
            precommits = roundState.votes.getPrecommits(vote.getRoundNumber());
            if (!precommits) throw PanicNullVoteSet(__FILE__, __LINE__);
            clog(dev::VerbosityInfo, Logger::channelTm) << "Added to precommit" << "vote" << vote.toString()
                                                        << "precommits"
                                                        << precommits->toStringShort();

            blockId = precommits->twoThirdMajority();
            if (blockId.is_initialized()) {
                if (blockId->getHash().empty()) {
                    enterNewRound(roundState.height, vote.getRoundNumber() + 1);
                } else {
                    enterNewRound(roundState.height, vote.getRoundNumber());
                    enterPrecommit(roundState.height, vote.getRoundNumber());
                    enterCommit(roundState.height, vote.getRoundNumber());

                    if (consensusConfig.isSkipTimeoutCommit() && precommits->hasAll()) {
                        // if we have all the votes now,
                        // go straight to new round (skip timeout commit)
                        // cs.scheduleTimeout(time.Duration(0), cs.Height, 0, cstypes.RoundStepNewHeight)
                        enterNewRound(roundState.height, 0);
                    }
                }
            } else if (roundState.roundNumber <= vote.getRoundNumber() && precommits->hasTwoThirdsAny()) {
                enterNewRound(roundState.height, vote.getRoundNumber());
                enterPrecommit(roundState.height, vote.getRoundNumber());
                enterPrecommitWait(roundState.height, vote.getRoundNumber());
            }
            break;
        default:
            throw ErrorUnexpectedVoteType(Vote::voteTypeToString(vote.getType()), __FILE__,
                                          __LINE__); // go-wire should prevent this. */
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
        clog(dev::VerbosityInfo, Logger::channelTm) << "Updating ValidBlock because of POL." << "validRound" <<
                                                    roundState.validRoundNumber << "POLRound" <<
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
        clog(dev::VerbosityInfo, Logger::channelTm) << "Unlocking because of POL." << "lockedRound"
                                                    << roundState.lockedRoundNumber << "POLRound" <<
                                                    vote.getRoundNumber();
        roundState.setLockedRoundNumber(0);
        roundState.setLockedBlock(nullptr);
        //roundState.setLockedBlockParts(NULL);
        eventBus.publishEventUnlock(roundState.roundStateEvent());
    }
}


void ConsensusState::doPrevote(height_t _height, int _roundNumber) {
    Logger::logRound(_height, _roundNumber);
// If a block is locked, prevote that.
    if (roundState.lockedBlock != nullptr) {
        clog(dev::VerbosityInfo, Logger::channelTm) << "enterPrevote: Block was locked";
        signAddVote(VoteTypePrevote, roundState.lockedBlock.get()->getBytes());
        return;
    }

    // If ProposalBlock is nil, prevote nil.
    if (roundState.proposalBlock == nullptr) {
        clog(dev::VerbosityInfo, Logger::channelTm) << "enterPrevote: ProposalBlock is nil";
        signAddVote(VoteTypePrevote, HexBytes());
        return;
    }

    //Validate proposal block
    int validationError = (controller.validateBlock(roundState.proposalBlock.get()->getBytes()));
    if (validationError < 0) {
        // ProposalBlock is invalid, prevote nil.
        clog(dev::VerbosityError, Logger::channelTm) << "enterPrevote: ProposalBlock is invalid" << "err"
                                                     << validationError;
        signAddVote(VoteTypePrevote, HexBytes());
        return;
    }

    // Prevote cs.ProposalBlock
    // NOTE: the proposal signature is validated when it is received,
    // and the proposal block parts are validated as they are received (against the merkle hash in the proposal)
    clog(dev::VerbosityInfo, Logger::channelTm) << "enterPrevote: ProposalBlock is valid";
    signAddVote(VoteTypePrevote, roundState.proposalBlock.get()->getBytes());
}


/** Enter: any +2/3 precommits for next round.*/
void ConsensusState::enterPrecommitWait(height_t height, int round) {
    if (roundState.height != height || round < roundState.roundNumber ||
        (roundState.roundNumber == round && RoundStepPrecommitWait <= roundState.stepType)) {
        clog(dev::VerbosityDebug, Logger::channelTm)
            << "enterPrecommitWait(%v/%v): Invalid args. Current step: %v/%v/%v"
            << height << round << roundState.height << roundState.roundNumber
            << RoundState::stateTypeString(roundState.stepType);
        return;
    }
    if (!roundState.votes.getPrecommits(round)->hasTwoThirdsAny()) {
        throw PanicEnterRound("enterPrecommitWait(" + to_string(height) + " " + to_string(round) +
                              ") but Precommits does not have any +2/3 votes", __FILE__, __LINE__);
    }
    clog(dev::VerbosityInfo, Logger::channelTm) << "enterPrecommitWait(%v/%v). Current: %v/%v/%v" << height << round <<
                                                roundState.height << roundState.roundNumber
                                                << RoundState::stateTypeString(roundState.stepType);
    auto guard = finally([&]() {
        roundState.updateRoundStep(roundState.roundNumber, RoundStepPrecommitWait);
        newStep();
    });

    // Wait for some more precommits; enterNewRound
    scheduleTimeout(consensusConfig.getTimeoutPrecommit(round), height, round, RoundStepPrecommitWait);

}

void ConsensusState::tryFinalizeCommit(height_t height) {
    Logger::logRound(height);

    if (roundState.height != height) {
        throw PanicEnterRound(
                "tryFinalizeCommit() cs.Height: " + to_string(roundState.height) + " vs height: " + to_string(height),
                __FILE__, __LINE__);
    }
    boost::optional<BlockID> blockID = roundState.votes.getPrecommits(
            roundState.commitRoundNumber)->twoThirdMajority();
    if (!blockID.is_initialized() || blockID->getHash().empty()) {
        clog(dev::VerbosityError, Logger::channelTm)
            << "Attempt to finalize failed. There was no +2/3 majority << or +2/3 was for <nil>.";
        return;
    }
    if (!roundState.proposalBlock->hashesTo(blockID->getHash())) {
        // TODO: this happens every time if we're not a validator (ugly logs)
        // TODO: ^^ wait, why does it matter that we're a validator?
        clog(dev::VerbosityInfo, Logger::channelTm) << "Attempt to finalize failed. We don't have the commit block."
                                                    << "proposal-block"
                                                    << roundState.proposalBlock->getHash().toString()
                                                    << "commit-block" << blockID->getHash().toString();
        return;
    }

    //	go
    finalizeCommit(height);
}

/* Increment height and goto cstypes.RoundStepNewHeight */
void ConsensusState::finalizeCommit(height_t _height) {
    if (roundState.height != _height || roundState.stepType != RoundStepCommit) {
        clog(dev::VerbosityDebug, Logger::channelTm) << "finalizeCommit(%v): Invalid args. Current step: %v/%v/%v"
                                                     << _height
                                                     << roundState.height << roundState.roundNumber
                                                     << RoundState::stateTypeString(roundState.stepType);
        return;
    }
    boost::optional<BlockID> blockID = roundState.votes.getPrecommits(
            roundState.commitRoundNumber)->twoThirdMajority();
    Block block = *roundState.proposalBlock.get();
    //blockParts = roundState.proposalBlockParts;
    if (!blockID.is_initialized()) {
        throw PanicNoPolka("Cannot finalizeCommit, ", __FILE__, __LINE__);
    }
    /*if (!blockParts.HasHeader(blockID.PartsHeader)) {
        throw Panic("Expected ProposalBlockParts header to be commit header", __FILE__, __LINE__);
    }*/
    if (!block.hashesTo(blockID->getHash())) {
        throw PanicInvalidHash("Cannot finalizeCommit, ProposalBlock does not hash to commit hash", __FILE__, __LINE__);
    }
    /*if (err = roundState.blockExec.ValidateBlock(roundState.state, block); err != nil) {
        throw PanicConsensus("+2/3 committed an invalid block: %v", err, __FILE__, __LINE__);
    }*/

    clog(dev::VerbosityInfo, Logger::channelTm) << "Finalizing commit of block with height "
                                                << to_string(block.getHeight())
                                                << " hash " << block.getBlockHash().toString()
                                                << " root ", block.getAppHash().toString();
    clog(dev::VerbosityInfo, Logger::channelTm) << "%v" << block.getBlockHash().toString();

    //todo WTF fail.Fail(); // XXX

    // Save to blockStore.
    /*if (controller.currentHeight() < block.get()->getHeight()) { //XXX previously roundState.blockStore._height() < block._height*/
    // NOTE: the seenCommit is local justification to commit this block,
    // but may differ from the LastCommit included in the next block
    controller.append(block.getBytes(),
                      roundState.votes.getPrecommits(roundState.commitRoundNumber)->getSignatures());
    /*} else {
        // Happens during replay if we already saved the block but didn't commit
        clog(dev::VerbosityInfo, Logger::channelTm) << "Calling finalizeCommit on already stored block" << "_height" << block._height);
    }*/

    // fail.Fail(); // XXX

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
    //roundState.wal.WriteSync(EndHeightMessage{_height}); // NOTE: fsync
/*
    //fail.Fail(); // XXX

    // Create a copy of the state for staging and an event cache for txs.
    stateCopy = roundState.state.Copy();

    // Execute and commit the block, update and save the state, and update the mempool.
    // NOTE The block.AppHash wont reflect these txs until the next block.
    var err error
    stateCopy, err = roundState.blockExec.ApplyBlock(stateCopy, types.BlockID{block.Hash(), blockParts.Header()}, block)
    if (err != nil) {
        clog(dev::VerbosityError, Logger::channelTm) << "Error on ApplyBlock. Did the application crash? Please restart tendermint" << "err" << err);
        err = cmn.Kill();
        if (err != nil) {
            clog(dev::VerbosityError, Logger::channelTm) << "Failed to kill this process - please do so manually" << "err" << err);
        }
        return;
    }

    fail.Fail(); // XXX
*/
    // NewHeightStep!
    updateToState(state.get());

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
    return (roundState.validators.hasAddress(privValidator->getAddress()));
}

bool ConsensusState::isProposer() {
    return roundState.validators.getProposer()->getAddress() == privValidator->getAddress();
}


void ConsensusState::decideProposal(height_t height, int round) {

    boost::optional<Block> block;

    // Decide on block
    if (roundState.lockedBlock != nullptr) {
        block = *roundState.lockedBlock.get();
    } else if (roundState.validBlock != nullptr) {
        block = *roundState.validBlock.get();
    } else {
        block = controller.getBlockToPropose();
    }

    // Make proposal
    BlockID polBlockID;
    int polRound = roundState.votes.polInfo(polBlockID);
    Proposal proposal(height, round, boost::posix_time::second_clock::local_time(), polRound, polBlockID);
    try {
        privValidator->signProposal(state->getChainID(), proposal);
    } catch (SignError &e) {
        if (!replayMode) {
            clog(dev::VerbosityError, Logger::channelTm) << "enterPropose: Error signing proposal" << "height" << height
                                                         << "round" << round << "err" << e.getDescription();
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
    sendInternalMessage(
            BlockMessage(AddressTm(std::vector<uint8_t>()), polBlockID, roundState.height, roundState.roundNumber,
                         block.get()));

    clog(dev::VerbosityInfo, Logger::channelTm) << "Signed proposal" << "height" << height << "round" << round
                                                << "proposal"
                                                << proposal.toString();
    clog(dev::VerbosityDebug, Logger::channelTm) << "Signed proposal block: %v" << block->getHash().toString();
}

const RoundState &ConsensusState::getRoundState() const {
    return roundState;
}

void ConsensusState::handleBlockMsg(const BlockMessage &msg) {
    cout << msg.toString(); //TODO unimplemented
}

void ConsensusState::handleTimeout(const TickerMessage &msg) {
    clog << "Received tock" << "timeout" << msg.getDuration() << "height" << msg.getHeight() << "round"
         << msg.getRoundNumber() << "step" << RoundState::stateTypeString(msg.getStepType());

    // timeouts must be for current height, round, step
    if (msg.getHeight() != roundState.height || msg.getRoundNumber() < roundState.roundNumber ||
        (msg.getRoundNumber() == roundState.roundNumber && msg.getStepType() < roundState.stepType)) {
        clog(dev::VerbosityInfo, Logger::channelTm) << "Ignoring tock because we're ahead" << "height"
                                                    << roundState.height << "round" << roundState.roundNumber <<
                                                    "step" << RoundState::stateTypeString(roundState.stepType);
        return;
    }

    // the timeout will now cause a state transition
    std::lock_guard<std::mutex> lock(mtx);

    switch (msg.getStepType()) {
        case RoundStepNewHeight:
            // NewRound event fired from enterNewRound.
            // XXX: should we fire timeout here (for timeout commit)?
            enterNewRound(msg.getHeight(), 0);
            break;
        case RoundStepNewRound:
            enterPropose(msg.getHeight(), 0);
            break;
        case RoundStepPropose:
            eventBus.publishEventTimeoutPropose(EventDataRoundState(roundState.height, roundState.roundNumber,
                                                                    RoundState::stateTypeString(roundState.stepType)));
            enterPrevote(msg.getHeight(), msg.getRoundNumber());
            break;
        case RoundStepPrevoteWait:
            eventBus.publishEventTimeoutWait(EventDataRoundState(roundState.height, roundState.roundNumber,
                                                                 RoundState::stateTypeString(roundState.stepType)));
            enterPrecommit(msg.getHeight(), msg.getRoundNumber());
            break;
        case RoundStepPrecommitWait:
            eventBus.publishEventTimeoutWait(EventDataRoundState(roundState.height, roundState.roundNumber,
                                                                 RoundState::stateTypeString(roundState.stepType)));
            enterNewRound(msg.getHeight(), msg.getRoundNumber() + 1);
            break;
        default:
            throw Panic("Invalid timeout step:" + RoundState::stateTypeString(msg.getStepType()), __FILE__, __LINE__);
    }
}

void ConsensusState::signAddVote(VoteType type, const HexBytes &b) {
    // if we don't have a key or we're not in the validator set, do nothing
    if (!privValidator.is_initialized() || !roundState.validators.hasAddress(privValidator->getAddress())) {
        return;
    }
    Vote vote = signVote(type, b);
    sendInternalMessage(VoteMessage(AddressTm(), vote));
    clog(dev::VerbosityInfo, Logger::channelTm) << "Signed and pushed vote" << "height" << roundState.height << "round"
                                                << roundState.roundNumber << "vote" << vote.toString();
//    if !cs.replayMode {
//    clog(dev::VerbosityError, Logger::channelTm) << "Error signing vote" << "height" << roundState.height << "round"
//                                                 << roundState.roundNumber << "vote" << vote.toString();
//    }
}

Vote ConsensusState::signVote(VoteType type, const HexBytes &b) {
    AddressTm addr = privValidator->getAddress();

    boost::optional<Validator> validator = roundState.validators.getByAddress(addr);
    int valIndex = validator.is_initialized() ? validator->getIndex() : 0;
    Vote vote(
            addr, //ValidatorAddress
            valIndex, //ValidatorIndex
            roundState.height, //Height
            roundState.roundNumber, //Round
            boost::posix_time::second_clock::local_time(), //Timestamp
            type, //        Type
            BlockID(b)// , header) //BlockID
    );
    privValidator->signVote(state->getChainID(), vote);
    return vote;
}

