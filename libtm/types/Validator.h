//
// Created by Maru on 7/9/2018.
//

#ifndef SRC_VALIDATOR_H
#define SRC_VALIDATOR_H


#include <cstdint>
#include <vector>
#include "HexBytes.h"


class Proposal;

class Validator {
    HexBytes address;
    Pubkey pubKey;
    int64_t votingPower;
    int64_t accum;

public:

    Validator(Pubkey pubKey, int64_t votingPower);

    void signProposal(const std::string &chainID, Proposal &proposal);

    const HexBytes &getAddress() const;

    const Pubkey &getPubKey() const;

    int64_t getVotingPower() const;

    int64_t getAccum() const;

    std::string toString();

};

class ValidatorSet {
    std::vector<Validator> validators;
public:

    ValidatorSet(const std::vector<Validator> &validators);

    Validator getByIndex(int index);

    HexBytes hash() const;

    unsigned int size();

    Validator getProposer() const;

    ValidatorSet copy();

    void incrementAccum(int i);

    bool hasAddress(const HexBytes bytes);
};


#endif //SRC_VALIDATOR_H
