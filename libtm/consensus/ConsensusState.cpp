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
ConsensusState::ConsensusState(ConsensusConfig _config) : eventSwitch(EventSwitch()) {
    consensusConfig = _config;//TODO
    // timeoutTicker = TODO newTimeoutTicker();
    //doWALCatchup = true;
    //TODO wal = NULL; //TODO nilWAL{};
}

ConsensusState::ConsensusState(ConsensusConfig _config, State _state) : ConsensusState(_config) {
    updateToState(_state);
    // Don't call scheduleRound0 yet We do that upon Start().
    reconstructLastCommit(_state);
    //cs.setBaseService(newBaseService(nil, "ConsensusState", cs));
}


ConsensusState::~ConsensusState() {
    // TODO Auto-generated destructor stub
}


void
ConsensusState::setProposal(Proposal _proposal) { //throw(ErrInvalidProposalPolRound, ErrorInvalidProposalSignature) {
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
        throw ErrorInvalidProposalSignature();
    }

    //TODO cs.ProposalBlockParts = types.NewPartSetFromHeader(proposal.BlockPartsHeader)
    logInfo("Received proposal", _proposal.toString());
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
        logInfo("Ignoring updateToState()", "newHeight", _state.getLastBlockHeight() + 1, "oldHeight",
                this->state.getLastBlockHeight() + 1);
        newStep();
        return;
    }

    // Reset fields based on state.
    ValidatorSet validators = _state.getValidators();
    shared_ptr<VoteSet> lastPrecommits; //FIXME
    if (roundState.commitRoundNumber > -1 && roundState.votes.isEmpty()) {
        if (!roundState.votes.getPrecommits(roundState.commitRoundNumber).get()->hasTwoThirdMajority()) {
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
    auto t = roundState.commitTime.isZero() ?
             consensusConfig.commit(Time()) :
             consensusConfig.commit(roundState.commitTime);
    roundState.startTime = Time(t);

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
    //TODO Commit seenCommit = blockStore.loadSeenCommit(state.lastBlockHeight);
    Commit seenCommit = loadSeenCommit(state.getLastBlockHeight());
    VoteSet lastPrecommits = VoteSet(state.getChainID(), state.getLastBlockHeight(), seenCommit.round(),
                                     VoteTypePrecommit, state.getLastValidators());
    for (uint i = 0; i < seenCommit.getPrecommits().size(); ++i) {
        Vote precommit = *seenCommit.getPrecommits()[i].get();
        //if(precommit == NULL ) continue ;;
        try {
            lastPrecommits.addVote(precommit);
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
    if (roundState.proposal->isEmpty()
        //TODO || proposalBlock == NULL
            )
        return false;
    // we have the proposal. if there's a POLRound,
    // make sure we have the prevotes from it too
    if (proposal->getPolRound() < 0)
        return true;
    // if there's no pol, the proposer is lying or we haven't received it yet (it should exist if there's 2/3 majority)
    return roundState.votes.getPrevotes(proposal->getPolRound()).hasTwoThirdMajority();
}







