//
// Created by Maru on 7/9/2018.
//

#include <cstdint>
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
