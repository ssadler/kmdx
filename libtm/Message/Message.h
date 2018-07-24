//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_MESSAGE_H
#define TM_LIGHT_MESSAGE_H

#include "../types/Proposal.h"
#include "../types/Vote.h"

class PeerID : public HexBytes {
};

class Message {
    PeerID peerID;
public:
    const PeerID &getPeerID() const {
        return peerID;
    }
};

class VoteMessage : public Message {
    Vote vote;
public:
    const Vote &getVote() const;
};

class ProposalMessage : public Message {
    Proposal proposal;
public:
    const Proposal getProposal() const;
};

class BlockMessage : public Message {
    Block block;
public:
    const Block getBlock() const;
};

#endif //TM_LIGHT_MESSAGE_H
