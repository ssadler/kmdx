//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_VOTEMESSAGE_H
#define TM_LIGHT_VOTEMESSAGE_H


#include "../types/Vote.h"
#include "Message.h"

class VoteMessage : public Message {
    Vote vote;
public:
    const Vote &getVote() const;
};


#endif //TM_LIGHT_VOTEMESSAGE_H
