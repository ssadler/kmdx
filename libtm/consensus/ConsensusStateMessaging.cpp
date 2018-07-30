//
// Created by utnso on 20/07/18.
//

#include "ConsensusState.h"

void ConsensusState::handleProposalMsg(
        const ProposalMessage &msg) {// throw(ErrInvalidProposalPolRound, ErrorInvalidProposalSignature) {
    // will not cause transition.
    // once proposal is set, we can receive block parts
    setProposal(msg.getProposal()); //throws ErrInvalidProposalPolRound, ErrorInvalidProposalSignature
}

// attempt to add the vote and dupeout the validator if its a duplicate signature
// if the vote gives us a 2/3-any or 2/3-one, we transition
void ConsensusState::handleVoteMsg(const VoteMessage &msg) {
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

void ConsensusState::recover() {
    //TODO
}

/** receiveRoutine handles messages which may cause state transitions.
* it's argument (n) is the number of messages to process before exiting - use 0 to run forever
* It keeps the RoundState and is the only thing that updates it.
* Updates (state transitions) happen on timeouts, complete proposals, and 2/3 majorities.
* ConsensusState must be locked before any internal state is updated.*/
void ConsensusState::receiveRoutine(dev::RLP &rlp) { //FIXME replace with use of channels
    auto guard = finally([&] {
        try {
            recover();
        } catch (exception &e) {
            //clog(dev::VerbosityError, channelTm) << "CONSENSUS FAILURE!!!", "err", r, "stack", string(debug.Stack()))
            throw e;
        }
    });
    /*while(1) { //steps are only used for testing
        if (maxSteps > 0) {
            if nSteps >= maxSteps {
                        cs.Logger.Info("reached max steps. exiting receive routine")
                        cs.nSteps = 0
                        return
                }
        }*/

    switch (Message::allTypes[rlp[0].toInt()]) {
        case MessageType::VoteType:
            handleVoteMsg(VoteMessage::fromRLP(rlp));
            break;
        case MessageType::ProposalType:
            handleProposalMsg(ProposalMessage::fromRLP(rlp));
            break;
        case MessageType::BlockType:
            handleBlockMsg(BlockMessage::fromRLP(rlp));
            break;
        case MessageType::TickerType:
            //tockChan?
            /*wal.Write(ti);
             // if the timeout is relevant to the rs
             // go to the next step
             handleTimeout(dynamic_pointer_cast<TickerMessage>, roundState);
             break;*/
        case MessageType::QuitType:
            return;
            /*quit():

             // NOTE: the internalMsgQueue may have signed messages from our
             // priv_val that haven't hit the WAL, but its ok because
             // priv_val tracks LastSig

             // close wal now that we're done writing to it
             cs.wal.Stop()
             cs.wal.Wait()

             close(cs.done)
             return*/
    }
}


