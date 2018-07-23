//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_PROPOSALMESSAGE_H
#define TM_LIGHT_PROPOSALMESSAGE_H


#include "../types/Proposal.h"
#include "Message.h"

class ProposalMessage : public Message {
    Proposal proposal;
public:
    const Proposal &getProposal() const;
};


#endif //TM_LIGHT_PROPOSALMESSAGE_H
