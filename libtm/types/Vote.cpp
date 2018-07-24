//
// Created by utnso on 12/07/18.
//

#include "Vote.h"

const HexBytes &Vote::getValidatorAddress() const {
    return validatorAddress;
}

int Vote::getValidatorIndex() const {
    return validatorIndex;
}

int64_t Vote::getHeight() const {
    return height;
}

int Vote::getRoundNumber() const {
    return roundNumber;
}

time_t Vote::getTimestamp() const {
    return timestamp;
}

VoteType Vote::getType() const {
    return type;
}

const BlockID &Vote::getBlockID() const {
    return blockID;
}

const Signature &Vote::getSignature() const {
    return signature;
}

Vote::Vote(VoteType _type) {
    this->type = _type;
}
