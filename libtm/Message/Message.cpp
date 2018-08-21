//
// Created by utnso on 16/07/18.
//

#include "Message.h"

using byte = uint8_t;

Address Message::getAddress() const {
    return address;
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

ProposalMessage::ProposalMessage(const Address &address, const Proposal &proposal)
        : Message(address, MessageType::ProposalType),
          proposal(proposal) {}

ProposalMessage::ProposalMessage(const Proposal &proposal) : ProposalMessage(Address(), proposal) {}


VoteMessage VoteMessage::fromRLP(dev::RLP &r) {
    return VoteMessage(Address(r[0].toVector<uint8_t>()), Vote::fromRLP(r));
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
            /*default:
                return "unknown message type";*/
    }
    return "unknown message type";
}

constexpr MessageType Message::allTypes[5];

/*= {MessageType::VoteType, MessageType::ProposalType, MessageType::BlockType,
                     MessageType::TickerType, MessageType::QuitType};*/

MessageType Message::typeFromString(std::string text) {
    if (text == "VoteType") return MessageType::VoteType;
    if (text == "ProposalType") return MessageType::ProposalType;
    if (text == "BlockType") return MessageType::BlockType;
    if (text == "Ticker") return MessageType::TickerType;
    return MessageType::QuitType;

}

Message::Message(const Address address, MessageType messageType) : address(address), messageType(messageType) {}

std::string Message::toString() const {
    return "{Message:{type:" + typeToString(messageType) + "},Address:{" + getAddress().toString() + "}}";
}

dev::u256 VoteMessage::toRLP() {
    dev::RLPStream rlp(2);
    rlp.append(this->getAddress());
    rlp.append(this->vote.toRLP());
    return dev::u256(rlp.out());
}

VoteMessage::VoteMessage(const Address address, const Vote &vote) : Message(address,
                                                                            MessageType::VoteType),
                                                                    vote(vote) {}

const Block BlockMessage::getBlock() const {
    return block;
}

BlockMessage::BlockMessage(const Address &address, const BlockID &blockID, const int64_t &height, int roundNumber,
                           const Block &block)
        : Message(address, MessageType::BlockType), block(block), blockID(blockID), height(height),
          roundNumber(roundNumber) {}

BlockMessage BlockMessage::fromRLP(dev::RLP &) {
//    BlockID blockID;
    return BlockMessage(Address(), BlockID(), int64_t(-1), -1, Block()); //TODO unimplemented
}

TickerMessage::TickerMessage(const Address &address) : Message(address, MessageType::TickerType) {

}
