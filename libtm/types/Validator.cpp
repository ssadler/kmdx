//
// Created by Maru on 7/9/2018.
//

#include <cstdint>
#include "Validator.h"

Validator::Validator(Pubkey _pubKey, int64_t _votingPower) {
    pubKey = _pubKey;
    votingPower = _votingPower;
    address = pubKey.getAddress();
    accum = 0;
}