//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_MESSAGE_H
#define TM_LIGHT_MESSAGE_H

#include <cstdint>
#include <libdevcore/RLP.h>
#include "../types/Proposal.h"
#include "../types/Vote.h"
#include "../types/RoundStepType.h"

using byte = uint8_t;


enum class MessageType {
    VoteType = 1, ProposalType = 2, BlockType = 3, TickerType = 4, QuitType = 5
};

class Message {
protected:
    AddressTm addresstm;
    MessageType messageType;
public:
    Message(const AddressTm addresstm, MessageType messageType);

    AddressTm getAddress() const;

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
    VoteMessage(const AddressTm addresstm, const Vote &vote);

    static VoteMessage fromRLP(dev::RLP &r);

    dev::u256 toRLP();

public:
    const Vote &getVote() const;

};

class ProposalMessage : public Message {
private:
    Proposal proposal;
public:
    ProposalMessage(const AddressTm &addresstm, const Proposal &proposal);

//for internal messages
    ProposalMessage(const Proposal &proposal);

    const Proposal getProposal() const;

    static ProposalMessage fromRLP(dev::RLP &r);

    dev::RLPStream toRLP();

};

class BlockMessage : public Message {
    Block block;
    BlockID blockID;
    height_t height;
    int roundNumber;
public:

    BlockMessage(const AddressTm &addresstm, const BlockID &blockID, const height_t &height, int roundNumber,
                 const Block &block);

    static BlockMessage fromRLP(dev::RLP &);

    const Block getBlock() const;
};

class TickerMessage : public Message {
    height_t height; // Height we are working on
    int roundNumber;
    RoundStepType stepType;
    time_duration duration;
public:
    TickerMessage(height_t height, unsigned int roundNumber,
                  RoundStepType stepType, const time_duration &duration);

    static TickerMessage fromRLP(dev::RLP);

    dev::RLPStream toRLP();

    height_t getHeight() const;

    int getRoundNumber() const;

    RoundStepType getStepType() const;

    const time_duration &getDuration() const;
};

#endif //TM_LIGHT_MESSAGE_H
