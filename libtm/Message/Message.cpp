//
// Created by utnso on 16/07/18.
//

#include "Message.h"

using byte = uint8_t;

const PeerID Message::getPeerID() const {
    return peerID;
}

MessageType Message::getMessageType() const {
    return messageType;
}

const Vote &VoteMessage::getVote() const {
    return vote;
}

const Proposal ProposalMessage::getProposal() const {
    return proposal;
}

VoteMessage VoteMessage::fromRLP(dev::RLP &r) {
    return VoteMessage(PeerID(r[0].toVector<uint8_t>()), Vote::fromRLP(r));
}

std::string Message::typeToString(MessageType type) {
    switch (type) {
        case MessageType::VoteType :
            return "VoteType";
        case MessageType::ProposalType :
            return "ProposalType";
        case MessageType::BlockType :
            return "BlockType";
        case MessageType::TickerType :
            return "TickerType";
        case MessageType::QuitType :
            return "QuitType";
        default:
            return "unknown message type";
    }
}

MessageType Message::typeFromString(std::string text) {
    if (text == "VoteType") return MessageType::VoteType;
    if (text == "ProposalType") return MessageType::ProposalType;
    if (text == "BlockType") return MessageType::BlockType;
    if (text == "Ticker") return MessageType::TickerType;
    return MessageType::QuitType;

}

dev::u256 VoteMessage::toRLP() {
    dev::RLPStream rlp(2);
    rlp.append(this->getPeerID());
    rlp.append(this->vote.toRLP());
    return dev::u256(rlp.out());
}