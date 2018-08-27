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
//class Proposal;

class Validator {
    Address addresstm;
    PubKey pubKey;
    int64_t votingPower;
    int64_t accum;
    int index;

public:

    Validator(PubKey pubKey, int64_t votingPower);

    Validator compareAccum(const Validator &validator) const;

    void signProposal(const std::string &chainID, Proposal &proposal); //TODO

    const Address &getAddress() const;

    const PubKey &getPubKey() const;

    int64_t getVotingPower() const;

    int64_t getAccum() const;

    std::string toString() const;

    int getIndex() const;

};

class ValidatorSet {
    std::vector<Validator> validators;
    boost::optional<Validator> proposer;

public:
    const static unsigned int voteSetLength = 64;

    ValidatorSet();

    ValidatorSet(const std::vector<Validator> &validators);

    boost::optional<Validator> getByIndex(int index);

    boost::optional<Validator> getByAddress(Address addresstm);

    HexBytes hash() const;

    unsigned int getTotalVotingPower();

    unsigned long size();

    const boost::optional<Validator> getProposer();

    void incrementAccum(int i);

    bool hasAddress(const HexBytes bytes);

    boost::optional<Validator> findProposer() const;
};

#endif //SRC_VALIDATOR_H
