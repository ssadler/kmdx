//
// Created by Maru on 7/9/2018.
//

#include "Validator.h"

Validator::Validator(Pubkey _pubKey, int64_t _votingPower) : address(_pubKey.getAddress()), pubKey(_pubKey){
    votingPower = _votingPower;
    accum = 0;
}


bool ValidatorSet::hasAddress(const HexBytes bytes) {
    for (Validator v : validators) {
        if (v.getAddress() == bytes) return true;
    }
    return false;
}

Validator ValidatorSet::getByIndex(int index) {
    return this->validators.at(index);
}

unsigned int ValidatorSet::size() {
    return this->validators.size();
}

ValidatorSet::ValidatorSet(const std::vector<Validator> &validators) : validators(validators) {}


const HexBytes &Validator::getAddress() const {
    return address;
}

const Pubkey &Validator::getPubKey() const {
    return pubKey;
}

int64_t Validator::getVotingPower() const {
    return votingPower;
}

int64_t Validator::getAccum() const {
    return accum;
}

std::string Validator::toString() {
    return address.toString();
}