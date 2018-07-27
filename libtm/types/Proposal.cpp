//
// Created by utnso on 11/07/18.
//

#include "Proposal.h"

int64_t Proposal::getHeight() const {
    return height;
}


int Proposal::getPolRound() const {
    return polRound;
}

const Signature &Proposal::getSignature() const {
    return signature;
}

