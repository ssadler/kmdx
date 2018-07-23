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
int ConsensusState::setProposal(Proposal _proposal) {
    //TODO if (proposal.isEmpty()) return 0;
    if (_proposal.getHeight() != roundState.height || !(_proposal.getRoundState() == roundState)) {
        return 0;
    }

    // We don't care about the proposal if we're already in RoundStepCommit.
    if (stepType >= RoundStepCommit) {
        return 0;
    }

    // Verify POLRound, which must be -1 or between 0 and proposal.Round exclusive.
    if (_proposal.getPolRound() != -1 &&
        (_proposal.getPolRound() < 0 || _proposal.getPolRound() <= _proposal.getPolRound())) {
        throw new ErrInvalidProposalPolRound("");
    }

    // Verify signature
    if (!(state.getValidators().getProposer().getPubKey().verifyBytes(_proposal.signBytes(state.getChainID()),
                                                                      _proposal.getSignature()))) {
        throw new ErrorInvalidProposalSignature("");
    }

    //TODO cs.ProposalBlockParts = types.NewPartSetFromHeader(proposal.BlockPartsHeader)
    logInfo("Received proposal", _proposal.toString());
    return 0;
}

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


void ConsensusState::updateToState(State state) {
    //TODO
}

void ConsensusState::reconstructLastCommit(State state) {
    //TODO
}

/*void ConsensusState::setBaseService(BaseService baseService){
    //TODO
}*/

void receiveRoutine(const Message &msg, int MaxSteps) {
    //TODO exceptions
    //TODO maxSteps

}

void ConsensusState::handleMsg(const ProposalMessage msg) {
    // will not cause transition.
    // once proposal is set, we can receive block parts
    int error = setProposal(msg.getProposal());
}

// attempt to add the vote and dupeout the validator if its a duplicate signature
// if the vote gives us a 2/3-any or 2/3-one, we transition
void ConsensusState::handleMsg(const VoteMessage msg) {
    try {
        tryAddVote(msg.getVote(), msg.getPeerID());
    } catch (ErrAddingVote addingVote) {
        // TODO: punish peer
        // We probably don't want to stop the peer here. The vote does not
        // necessarily comes from a malicious peer but can be just broadcasted by
        // a typical peer.
        // https://github.com/tendermint/tendermint/issues/1281
    }

    // NOTE: the vote is broadcast to peers by the reactor listening
    // for vote events

    // TODO: If rs.Height == vote.Height && rs.Round < vote.Round,
    // the peer is sending us CatchupCommit precommits.
    // We could make note of this and help filter in broadcastHasVoteMessage().
}


/** Returns true if the proposal block is complete &&
* (if POLRound was proposed, we have +2/3 prevotes from there). */
bool ConsensusState::isProposalComplete() {
    if (proposal == NULL
        //TODO || proposalBlock == NULL
            )
        return false;
    // we have the proposal. if there's a POLRound,
    // make sure we have the prevotes from it too
    if (proposal->getPolRound() < 0)
        return true;
    // if there's no pol, the proposer is lying or we haven't received it yet (it should exist if there's 2/3 majority)
    return roundState.votes->getPrevotes(proposal->getPolRound()).hasTwoThirdMajority();
}


void ConsensusState::scheduleTimeout(int prevote, int64_t height, int number, RoundStepType type) {
    //TODO
}






