//
// Created by utnso on 16/07/18.
//
#include <cstdint>
#include "Message.h"

const Vote &VoteMessage::getVote() const {
    return vote;
}

const Proposal ProposalMessage::getProposal() const {
    return proposal;
}


