//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_MESSAGE_H
#define TM_LIGHT_MESSAGE_H

#include <cstdint>
#include <libdevcore/RLP.h>
#include "../types/Proposal.h"
#include "../types/Vote.h"

class PeerID : public HexBytes {
};


enum class MessageType {
    VoteType = 1, ProposalType = 2, BlockType = 3, TickerType = 4, QuitType = 5
};

class Message {
    PeerID peerID;
    MessageType messageType;
public:
    Message(const PeerID &peerID, MessageType messageType) : peerID(peerID), messageType(messageType) {}

    const PeerID &getPeerID() const {
        return peerID;
    }

    MessageType getMessageType() const {
        return messageType;
    }

    static std::string typeToString(MessageType type) {
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
        }
    }

    static constexpr MessageType allTypes[] = {MessageType::VoteType, MessageType::ProposalType, MessageType::BlockType,
                                               MessageType::TickerType, MessageType::QuitType};

    static MessageType typeFromString(std::string text) {
        if (text == "VoteType") return MessageType::VoteType;
        if (text == "ProposalType") return MessageType::ProposalType;
        if (text == "BlockType") return MessageType::BlockType;
        if (text == "Ticker") return MessageType::TickerType;
        return MessageType::QuitType;

    }
};

class VoteMessage : public Message {
    Vote vote;
public:
    VoteMessage(const PeerID &peerID, const Vote &vote) : Message(peerID,
                                                                  MessageType::VoteType),
                                                          vote(vote) {}

    static VoteMessage fromRLP(dev::RLP &);

    dev::RLP toRLP();
public:
    const Vote &getVote() const;

};

class ProposalMessage : public Message {
public:
    ProposalMessage(const Proposal &proposal);

    static ProposalMessage fromRLP(dev::RLP &);

private:
    Proposal proposal;
public:
    const Proposal getProposal() const;

    ProposalMessage(const PeerID &peerID, const Proposal &proposal)
            : Message(peerID, MessageType::ProposalType),
              proposal(proposal) {}
};

class BlockMessage : public Message {
    shared_ptr<Block> block;
    BlockID blockID;
    int64_t height;
    int roundNumber;
public:

    BlockMessage(const PeerID &peerID, const BlockID &blockID, const int64_t &height, int roundNumber,
                 shared_ptr<Block> block)
            : Message(peerID, MessageType::BlockType), block(block), blockID(blockID), height(height),
              roundNumber(roundNumber) {}

    static BlockMessage fromRLP(dev::RLP &);
    const Block getBlock() const;
};

class TickerMessage : public Message {
public:
    TickerMessage(const PeerID &peerID) : Message(peerID, MessageType::TickerType) {

    }

};

#endif //TM_LIGHT_MESSAGE_H
