//
// Created by Maru on 7/9/2018.
//

#ifndef SRC_VALIDATOR_H
#define SRC_VALIDATOR_H


#include <cstdint>
#include <vector>
#include <boost/optional.hpp>

#include "HexBytes.h"
#include "Error.h"
#include "Proposal.h"

class ValidatorSetTest;

class Validator {
    PubKey pubKey;
    AddressTm address;
    int64_t votingPower;
    int64_t accum;
    int index;

    friend class ValidatorSetTest;

public:

    Validator(PubKey pubKey, int64_t votingPower);

    Validator(PubKey pubKey, int64_t votingPower, int _index); //for testing
    Validator(dev::Secret secret, int64_t _votingPower);

    Validator(dev::Secret secret, int64_t _votingPower, int _index); //for testing
    Validator *compareAccum(Validator &validator);

    void signProposal(const std::string &chainID, Proposal &proposal); //TODO

    const AddressTm &getAddress() const;

    const PubKey &getPubKey() const;

    int64_t getVotingPower() const;

    int64_t getAccum() const;

    std::string toString() const;

    int getIndex() const;

    bool operator==(const Validator &other);

    void incrementAccum(int times);

    void decrementAccum(int64_t n);

//    int compareByAddress(const Validator& other) const;
    const HexBytes hash() const;

};

struct ValidatorCompareByAddress {
    bool operator()(const Validator &one, const Validator &other);
};

#endif //SRC_VALIDATOR_H
