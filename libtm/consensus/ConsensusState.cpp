/*
 * ConsensusState.cpp
 *
 * Created on: Jul 9, 2018
 * Author: Maru
 */

#include "ConsensusState.h"


/* ConsensusState handles execution of the main algorithm.
 * It processes votes and proposals, and upon reaching agreement,
 * commits blocks to the chain and executes them against the application.
 * The internal state machine receives input from peers, the internal validator, and from a timer.*/
ConsensusState::ConsensusState(ConsensusConfig _config) : consensusConfig(_config), timeoutTicker(), eventBus(),
                                                          eventSwitch() {
    doWALCatchup = true;
    //TODO wal = NULL; //TODO nilWAL{};
}

ConsensusState::ConsensusState(const ConsensusState &cs) : ConsensusState(cs.consensusConfig) {
    //FIXME
}

ConsensusState::ConsensusState(ConsensusConfig _config, State _state) : ConsensusState(_config) {
    updateToState(_state);
    // Don't call scheduleRound0 yet We do that upon Start().
    reconstructLastCommit(_state);
    //TODO cs.setBaseService(newBaseService(nil, "ConsensusState", cs));
}


ConsensusState::~ConsensusState() {
    // TODO Auto-generated destructor stub
}


void
ConsensusState::setProposal(Proposal _proposal) { //throw(ErrInvalidProposalPolRound, ErrorInvalidSignature) {
    // Already have one
    if (proposal != nullptr)
        return;

    //proposal doesnt apply
    if (_proposal.getHeight() != roundState.height || !(roundState == _proposal.getRoundState())) {
        return;
    }

    // We don't care about the proposal if we're already in RoundStepCommit.
    if (stepType >= RoundStepCommit) {
        return;
    }

    // Verify POLRound, which must be -1 or between 0 and proposal.Round exclusive.
    if (_proposal.getPolRound() != -1 &&
        (_proposal.getPolRound() < 0 || _proposal.getPolRound() <= _proposal.getPolRound())) {
        throw ErrInvalidProposalPolRound();
    }

    // Verify signature
    if (!(state.getValidators().getProposer().getPubKey().verifyBytes(_proposal.signBytes(state.getChainID()),
                                                                      _proposal.getSignature()))) {
        throw ErrorInvalidSignature("proposal");
    }
    clog(dev::VerbosityInfo, channelTm) << "Received proposal: " << _proposal.toString();
//    cs.ProposalBlockParts = types.NewPartSetFromHeader(proposal.BlockPartsHeader)
}

/* Updates ConsensusState and increments height to match that of state.
* The round becomes 0 and cs.Step becomes cstypes.RoundStepNewHeight. */
void ConsensusState::updateToState(State _state) {
    if (roundState.commitRoundNumber > -1 && 0 < roundState.height &&
        roundState.height != _state.getLastBlockHeight()) {
        throw Panic("updateToState() expected state height of %v but found %v", roundState.height,
                    _state.getLastBlockHeight());
    }
    if (!this->state.isEmpty() && this->state.getLastBlockHeight() + 1 != roundState.height) {
        // This might happen when someone else is mutating this->state.
        // Someone forgot to pass in _state.copy() somewhere?!
        throw Panic("Inconsistent this->state.lastBlockHeight+1 %v vs roundState.height %v",
                    this->state.getLastBlockHeight() + 1, roundState.height);
    }

    // If (state isn't further out than this->state, just ignor)e.
    // This happens when SwitchToConsensus() is called in the reactor.
    // We don't want to reset e.g. the Votes, but we still want to
    // signal the new round step, because other services (eg. mempool)
    // depend on having an up-to-date peer state!
    if (!this->state.isEmpty() && (_state.getLastBlockHeight() <= this->state.getLastBlockHeight())) {
        clog(dev::VerbosityInfo, channelTm) << "Ignoring updateToState()" << "newHeight"
                                            << _state.getLastBlockHeight() + 1 << "oldHeight"
                                            << this->state.getLastBlockHeight() + 1;
        newStep();
        return;
    }

    // Reset fields based on state.
    ValidatorSet validators = _state.getValidators();
    boost::optional<VoteSet> lastPrecommits;
    if (roundState.commitRoundNumber > -1 && roundState.votes.isEmpty()) {
        if (!roundState.votes.getPrecommits(roundState.commitRoundNumber).get().hasTwoThirdMajority()) {
            throw Panic("updateToState(state) called but last Precommit round didn't have +2/3");
        }
        lastPrecommits = roundState.votes.getPrecommits(roundState.commitRoundNumber);
    }

    // RoundState fields
    roundState.setHeight(_state.getLastBlockHeight() + 1);
    roundState.updateRoundStep(0, RoundStepNewHeight);
    // "Now" makes it easier to sync up dev nodes.
    // We add timeoutCommit to allow transactions
    // to be gathered for the first block.
    // And alternative solution that relies on clocks:
    //  roundState.startTime = _state.lastBlockTime.add(timeoutCommit)
    auto t = roundState.commitTime.is_not_a_date_time() ?
             boost::posix_time::second_clock::local_time() :
             roundState.commitTime;
    roundState.startTime = t + consensusConfig.getTimeoutCommit();

    roundState.validators = validators;
    roundState.proposal = nullptr;
    roundState.proposalBlock = nullptr;
    //roundState.proposalBlockParts = NULL;
    roundState.lockedRoundNumber = 0;
    roundState.lockedBlock = nullptr;
    //roundState.lockedBlockParts = NULL;
    roundState.validRoundNumber = 0;
    roundState.validBlock = nullptr;
    //roundState.validBlockParts = NULL;
    roundState.votes = HeightVoteSet(_state.getChainID(), roundState.height, validators);
    roundState.commitRoundNumber = -1;
    roundState.lastCommit = lastPrecommits;
    roundState.lastValidators = _state.getLastValidators();

    this->state = _state;

    // Finally, broadcast RoundState
    newStep();
}

/** Reconstruct LastCommit from SeenCommit, which we saved along with the block,
* (which happens even before saving the state) */
void ConsensusState::reconstructLastCommit(State state) {
    if (state.getLastBlockHeight() == 0) {
        return;
    }

    Commit seenCommit = loadSeenCommit(state.getLastBlockHeight());
    VoteSet lastPrecommits = VoteSet(state.getChainID(), state.getLastBlockHeight(), seenCommit.round(),
                                     VoteTypePrecommit, state.getLastValidators());
    for (uint i = 0; i < seenCommit.getPrecommits().size(); ++i) {
        shared_ptr<Vote> precommit = seenCommit.getPrecommits()[i];
        if (precommit == nullptr) continue;
        try {
            lastPrecommits.addVote(*precommit.get());
        } catch (Panic &p) {
            throw PanicCrisis("Failed to reconstruct LastCommit: %v", p);
        }
        if (!lastPrecommits.hasTwoThirdMajority()) {
            throw PanicNoPolka("Failed to reconstruct LastCommit: ");
        }
    }
}


/** Returns true if the proposal block is complete &&
* (if POLRound was proposed, we have +2/3 prevotes from there). */
bool ConsensusState::isProposalComplete() {
    if (//roundState == NULL ||
            roundState.proposalBlock == nullptr)
        return false;
    // we have the proposal. if there's a POLRound,
    // make sure we have the prevotes from it too
    if (proposal->getPolRound() < 0)
        return true;
    // if there's no pol, the proposer is lying or we haven't received it yet (it should exist if there's 2/3 majority)
    return roundState.votes.getPrevotes(proposal->getPolRound()).hasTwoThirdMajority();
}

// OnStart implements cmn.Service.
// It loads the latest state via the WAL, and starts the timeout and receive routines.
void ConsensusState::onStart() {
    eventSwitch.start();

/*    // we may set the WAL in testing before calling Start,
    // so only OpenWAL if its still the nilWAL
    if _, ok := cs.wal.(nilWAL); ok {
        walFile := cs.config.WalFile()
        wal, err := cs.OpenWAL(walFile)
        if err != nil {
            cs.Logger.Error("Error loading ConsensusState wal", "err", err.Error())
            return err
        }
        cs.wal = wal
    }*/

    // we need the timeoutRoutine for replay so
    // we don't block on the tick chan.
    // NOTE: we will get a build up of garbage go routines
    // firing on the tockChan until the receiveRoutine is started
    // to deal with them (by that point, at most one will be valid)
    timeoutTicker.start();

    // we may have lost some votes if the process crashed
    // reload from consensus log to catchup
    if (doWALCatchup) {
        try { catchupReplay(roundState.height); } catch (Panic &p) {
            clog(dev::VerbosityError, channelTm) << "Error on catchup replay. Proceeding to start ConsensusState anyway"
                                                 << "err" << p.getDesc();
            // NOTE: if we ever do return an error here,
            // make sure to stop the timeoutTicker
        }

    }

    // now start the receiveRoutine
    boost::thread::attributes attrs;
//attrs.set_stack_size((c_depthLimit - c_offloadPoint) * c_singleExecutionStackSize);

    boost::exception_ptr exception;
    boost::thread{attrs, [&] {
        try {
            receiveRoutine(); //originally 0 -> run forever
        }
        catch (...) {
            exception = boost::current_exception(); // Catch all exceptions to be rethrown in parent thread.
        }
    }
    }.join();

    if (exception)
        boost::rethrow_exception(exception);

    // schedule the first round!
    // use GetRoundState so we don't race the receiveRoutine for access
    scheduleRound0(getRoundState_copy());

}

// timeoutRoutine: receive requests for timeouts on tickChan and fire timeouts on tockChan
// receiveRoutine: serializes processing of proposoals, block parts, votes; coordinates state transitions
void ConsensusState::startRoutines() {
    timeoutTicker.start();
    boost::fibers::fiber receiveRoutineFb(&ConsensusState::onStart, this);//TODO replace with receiveRoutine
    receiveRoutineFb.join();
}

// Create the next block to propose and return it.
// We really only need to return the parts, but the block
// is returned for convenience so we can log the proposal block.
// Returns nil block upon error.
// NOTE: keep it side-effect free for clarity.
boost::optional<Block> ConsensusState::createProposalBlock() {
    boost::optional<Block> block; //TODO incomplete
    return block;
}