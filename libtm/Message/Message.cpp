//
// Created by utnso on 16/07/18.
//

#include "Message.h"

const Vote &VoteMessage::getVote() const {
    return vote;
}

const Proposal ProposalMessage::getProposal() const {
    return proposal;
}

VoteMessage VoteMessage::fromRLP(dev::RLP &r){
    return VoteMessage(PeerID(r[0].toVector<uint8_t>()), Vote::fromRLP(r));
}

dev::u256 VoteMessage::toRLP(){
    dev::RLPStream rlp(2);
    rlp.append(this->getPeerID().getBites());
    rlp.append(this->vote.toRLP());
    return dev::u256(rlp.out());
}

PeerID::PeerID(vector<uint8_t> bytes) : HexBytes(bytes) {
    //TODO
}
