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
/*ConsensusState::ConsensusState(ConsensusConfig _config, TmController &_controller) : consensusConfig(_config),
                                                                                     controller(_controller),
                                                                                     )
        //                                                                                     , privValidator(),
//                                                                                     timeoutTicker(),
//                                                                                     eventBus(),
//                                                                                     eventSwitch()
                                                                                     {
    doWALCatchup = true;
    //TODO wal = NULL; //TODO nilWAL{};
}

ConsensusState::ConsensusState(const ConsensusState &cs, TmController &_controller) : ConsensusState(cs.consensusConfig,
                                                                                                     _controller)
{
//FIXME
}*/

ConsensusState::ConsensusState(ConsensusConfig _config, State _state, TmController &_controller) : consensusConfig(
        _config), controller(_controller), roundState(_state.getChainID()) {
    receiveRoutineWorker.setConsensusState(this);
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
    if (_proposal.getHeight() != roundState.height || (roundState.roundNumber != _proposal.getRoundNumber())) {
        return;
    }

    // We don't care about the proposal if we're already in RoundStepCommit.
    if (stepType >= RoundStepCommit) {
        return;
    }

    // Verify POLRound, which must be -1 or between 0 and proposal.Round exclusive.
    if (_proposal.getPolRound() != -1 &&
        (_proposal.getPolRound() < 0 || _proposal.getPolRound() <= _proposal.getPolRound())) {
        throw ErrInvalidProposalPolRound(__FILE__, __LINE__);
    }

    // Verify signature
    if (!(state.get().getValidators().getProposer().get().getPubKey().verifyBytes(
            _proposal.signBytes(state.get().getChainID()),
            _proposal.getSignature()))) {
        throw ErrorInvalidSignature("proposal", __FILE__, __LINE__);
    }
    clog(dev::VerbosityInfo, Logger::channelTm) << "Received proposal: " << _proposal.toString();
//    cs.ProposalBlockParts = types.NewPartSetFromHeader(proposal.BlockPartsHeader)
}

/* Updates ConsensusState and increments height to match that of state.
* The round becomes 0 and cs.Step becomes cstypes.RoundStepNewHeight. */
void ConsensusState::updateToState(State _state) {
    if (roundState.commitRoundNumber > -1 && 0 < roundState.height &&
        roundState.height != _state.getLastBlockHeight()) {
        throw Panic("updateToState() expected state height of %s but found %s" + to_string(roundState.height) +
                    to_string(_state.getLastBlockHeight()), __FILE__, __LINE__);
    }
    if (this->state.is_initialized() && this->state.get().getLastBlockHeight() + 1 != roundState.height) {
        // This might happen when someone else is mutating this->state.
        // Someone forgot to pass in _state.copy() somewhere?!
        throw Panic("Inconsistent this->state.lastBlockHeight+1 %s vs roundState.height %s" +
                    to_string(this->state.get().getLastBlockHeight() + 1) + to_string(roundState.height), __FILE__,
                    __LINE__);
    }

    // If (state isn't further out than this->state, just ignor)e.
    // This happens when SwitchToConsensus() is called in the reactor.
    // We don't want to reset e.g. the Votes, but we still want to
    // signal the new round step, because other services (eg. mempool)
    // depend on having an up-to-date peer state!
    if (this->state.is_initialized() && (_state.getLastBlockHeight() <= this->state.get().getLastBlockHeight())) {
        clog(dev::VerbosityInfo, Logger::channelTm) << "Ignoring updateToState()" << "newHeight"
                                                    << _state.getLastBlockHeight() + 1 << "oldHeight"
                                                    << this->state.get().getLastBlockHeight() + 1;
        newStep();
        return;
    }

    // Reset fields based on state.
    ValidatorSet validators = _state.getValidators();
    boost::optional<VoteSet> lastPrecommits;
    /*validators := state.Validators
    lastPrecommits := (*types.VoteSet)(nil)
    if cs.CommitRound > -1 && cs.Votes != nil {
                if !cs.Votes.Precommits(cs.CommitRound).HasTwoThirdsMajority() {*/
    if (roundState.commitRoundNumber > -1 && roundState.votes.isEmpty()) {
        if (!roundState.votes.getPrecommits(roundState.commitRoundNumber).get().hasTwoThirdMajority()) {
            throw Panic("updateToState(state) called but last Precommit round didn't have +2/3", __FILE__, __LINE__);
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
//    HeightVoteSet hvs(_state.getChainID(), roundState.height, validators);
//    roundState.votes = hvs; //TODO whats better here?
    roundState.commitRoundNumber = -1;
    roundState.lastCommit = lastPrecommits.get(); //FIXME coulfail
    roundState.lastValidators = _state.getLastValidators();

    this->state = _state;

    // Finally, broadcast RoundState
    newStep();
}

/** LoadSeenCommit returns the locally seen Commit for the given height.
 * This is useful when we've seen a commit, but there has not yet been
 * a new block at `height + 1` that includes this commit in its block.LastCommit.*/
Commit ConsensusState::loadSeenCommit(int64_t height) {
/*Commit commit;
        bz := bs.db.Get(calcSeenCommitKey(height))
if len(bz) == 0 {
return nil
}
err := cdc.UnmarshalBinaryBare(bz, commit)
if err != nil {
panic(cmn.ErrorWrap(err, "Error reading block seen commit"))
}
return commit*/ //FIXME
    if (height > 0) return Commit();
    return Commit();
}


/** Reconstruct LastCommit from SeenCommit, which we saved along with the block,
* (which happens even before saving the state) */
void ConsensusState::reconstructLastCommit(State _state) {
    if (_state.getLastBlockHeight() == 0) {
        return;
    }

    Commit seenCommit = loadSeenCommit(_state.getLastBlockHeight());
    VoteSet lastPrecommits(_state.getChainID(), _state.getLastBlockHeight(), seenCommit.round(),
                           VoteTypePrecommit, _state.getLastValidators());
    for (auto const &iterator : seenCommit.getPrecommits()) {
        const Vote &precommit = iterator.second;
        try {
            lastPrecommits.addVote(precommit);
        } catch (Panic &p) {
            throw PanicCrisis("Failed to reconstruct LastCommit:", p, __FILE__, __LINE__);
        }
        if (!lastPrecommits.hasTwoThirdMajority()) {
            throw PanicNoPolka("Failed to reconstruct LastCommit: ", __FILE__, __LINE__);
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
    return roundState.votes.getPrevotes(proposal->getPolRound())->hasTwoThirdMajority();
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
    /* if (doWALCatchup) {
         try { catchupReplay(roundState.height); } catch (Panic &p) {
             clog(dev::VerbosityError, Logger::channelTm)
                 << "Error on catchup replay. Proceeding to start ConsensusState anyway"
                 << "err" << p.getDesc();
             // NOTE: if we ever do return an error here,
             // make sure to stop the timeoutTicker
         }

     }*/


//attrs.set_stack_size((c_depthLimit - c_offloadPoint) * c_singleExecutionStackSize);
    receiveRoutineWorker.start(); //originally 0 -> run forever
    /*
    // now start the receiveRoutine
    boost::thread::attributes attrs;boost::exception_ptr exception;
    boost::thread{attrs, [&] {
        try {
            receiveRoutineWorker.start(); //originally 0 -> run forever
        }
        catch (...) {
            exception = boost::current_exception(); // Catch all exceptions to be rethrown in parent thread.
        }
    }
    }.join();

    if (exception)
        boost::rethrow_exception(exception);
*/
    // schedule the first round!
    // use GetRoundState so we don't race the receiveRoutine for access
    scheduleRound0(getRoundState());

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

void ConsensusState::addToPeerMsgQueue(Message m) {
    cout << m.toString();
    //TODO
}