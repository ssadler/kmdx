//
// Created by Maru on 7/9/2018.
//

#ifndef SRC_VALIDATOR_H
#define SRC_VALIDATOR_H


#include <cstdint>
#include "HexBytes.h"
#include "Signature.h"
#include <vector>



class Validator {
    HexBytes address;
    Pubkey pubKey;
    int64_t votingPower;
    int64_t accum;

    Validator(Pubkey pubKey, int64_t votingPower);

public:
    const HexBytes &getAddress() const {
        return address;
    }

    const Pubkey &getPubKey() const {
        return pubKey;
    }

    int64_t getVotingPower() const {
        return votingPower;
    }

    int64_t getAccum() const {
        return accum;
    }

    std::string toString() {
        return address.toString();
    }
};

class ValidatorSet {
    std::vector<Validator> validators;
public:
    HexBytes hash() const;

    Validator getProposer() const;

    ValidatorSet copy();

    void incrementAccum(int i);

    bool hasAddress(const HexBytes bytes);
};


#endif //SRC_VALIDATOR_H
