//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_MESSAGE_H
#define TM_LIGHT_MESSAGE_H

#include <cstdint>
#include <libdevcore/RLP.h>
#include "../types/Proposal.h"
#include "../types/Vote.h"

using byte = uint8_t;


enum class MessageType {
    VoteType = 1, ProposalType = 2, BlockType = 3, TickerType = 4, QuitType = 5
};

class Message {
    Address address;
    MessageType messageType;
public:
    Message(const Address address, MessageType messageType);

    Address getAddress() const;

    MessageType getMessageType() const;

    static std::string typeToString(MessageType type);


    static constexpr MessageType allTypes[] = {MessageType::VoteType, MessageType::ProposalType, MessageType::BlockType,
                                               MessageType::TickerType, MessageType::QuitType};

    static MessageType typeFromString(std::string text);

    std::string toString() const;

};

class VoteMessage : public Message {
    Vote vote;
public:
    VoteMessage(const Address address, const Vote &vote);

    static VoteMessage fromRLP(dev::RLP &r);

    dev::u256 toRLP();

public:
    const Vote &getVote() const;

};

class ProposalMessage : public Message {
public:
    ProposalMessage(const Proposal &proposal);

    static ProposalMessage fromRLP(dev::RLP &) {
        //TODO unimplemented
        return ProposalMessage(Proposal(int64_t(0), -1, -1, BlockID()));
    }

private:
    Proposal proposal;
public:
    const Proposal getProposal() const;

    ProposalMessage(const Address &address, const Proposal &proposal);
};

class BlockMessage : public Message {
    Block block;
    BlockID blockID;
    int64_t height;
    int roundNumber;
public:

    BlockMessage(const Address &address, const BlockID &blockID, const int64_t &height, int roundNumber,
                 const Block &block);

    static BlockMessage fromRLP(dev::RLP &);

    const Block getBlock() const;
};

class TickerMessage : public Message {
public:
    TickerMessage(const Address &address);

};

#endif //TM_LIGHT_MESSAGE_H
