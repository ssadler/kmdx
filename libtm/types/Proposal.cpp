//
// Created by utnso on 11/07/18.
//

#include "Proposal.h"

int64_t Proposal::getHeight() const {
    return height;
}

const RoundState &Proposal::getRoundState() const {
    return roundState;
}

time_t Proposal::getTimestamp() const {
    return Timestamp;
}

const Block &Proposal::getBlock() const {
    return block;
}

int Proposal::getPolRound() const {
    return polRound;
}

const HexBytes &Proposal::getPolBlockID() const {
    return polBlockID;
}

const Signature &Proposal::getSignature() const {
    return signature;
}
