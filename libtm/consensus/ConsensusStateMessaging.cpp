//
// Created by utnso on 20/07/18.
//

#include "ConsensusState.h"

void ConsensusState::handleMsg(
        const ProposalMessage msg) {// throw(ErrInvalidProposalPolRound, ErrorInvalidProposalSignature) {
    // will not cause transition.
    // once proposal is set, we can receive block parts
    setProposal(msg.getProposal()); //throws ErrInvalidProposalPolRound, ErrorInvalidProposalSignature
}

// attempt to add the vote and dupeout the validator if its a duplicate signature
// if the vote gives us a 2/3-any or 2/3-one, we transition
void ConsensusState::handleMsg(const VoteMessage msg) {
    try {
        tryAddVote(msg.getVote(), msg.getPeerID());
    } catch (ErrAddingVote &addingVote) {
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


