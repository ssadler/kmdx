//
// Created by utnso on 16/07/18.
//

#include "Message.h"

using byte = uint8_t;

AddressTm Message::getAddress() const {
    return addresstm;
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

ProposalMessage::ProposalMessage(const AddressTm &addresstm, const Proposal &proposal)
        : Message(addresstm, MessageType::ProposalType),
          proposal(proposal) {}

ProposalMessage::ProposalMessage(const Proposal &proposal) : ProposalMessage(AddressTm(), proposal) {}


VoteMessage VoteMessage::fromRLP(dev::RLP &r) {
    return VoteMessage(AddressTm(r[0].toVector<uint8_t>()), Vote::fromRLP(r));
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

Message::Message(const AddressTm addresstm, MessageType messageType) : addresstm(addresstm), messageType(messageType) {}

std::string Message::toString() const {
    return "{Message:{type:" + typeToString(messageType) + "},Address:{" + getAddress().toString() + "}}";
}

dev::u256 VoteMessage::toRLP() {
    dev::RLPStream rlp(2);
    rlp.append(this->getAddress());
    //FIXME rlp.append(this->vote.toRLP());
    return dev::u256(rlp.out());
}

VoteMessage::VoteMessage(const AddressTm addresstm, const Vote &vote) : Message(addresstm,
                                                                                MessageType::VoteType),
                                                                        vote(vote) {}

const Block BlockMessage::getBlock() const {
    return block;
}

BlockMessage::BlockMessage(const AddressTm &addresstm, const BlockID &blockID, const height_t &height, int roundNumber,
                           const Block &block)
        : Message(addresstm, MessageType::BlockType), block(block), blockID(blockID), height(height),
          roundNumber(roundNumber) {}

BlockMessage BlockMessage::fromRLP(dev::RLP &) {
//    BlockID blockID;
    return BlockMessage(AddressTm(), BlockID(), -1, -1, Block()); //TODO unimplemented
}

TickerMessage::TickerMessage(const AddressTm &addresstm) : Message(addresstm, MessageType::TickerType) {

}
