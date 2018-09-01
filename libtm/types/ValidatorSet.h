//
// Created by thpn on 28/08/18.
//

#ifndef AYETH_VALIDATORSET_H
#define AYETH_VALIDATORSET_H

#include <cstdint>
#include <vector>
#include <iterator>
#include <boost/optional.hpp>
//#include <algorithm>

#include "HexBytes.h"
#include "Error.h"
#include "Proposal.h"
#include "Validator.h"

class ValidatorSetTest;

class ValidatorSet {
    std::vector<Validator> validators;
    Validator *proposer;
    std::mutex mtx;

    friend class ValidatorSetTest;

public:
    const static unsigned int voteSetLength = 64;

    ValidatorSet();

    ValidatorSet(const std::vector<Validator> &validators);

    ValidatorSet(const std::vector<Validator> &_validators, Validator *_proposer);

    ValidatorSet(const ValidatorSet &other);

    ValidatorSet &operator=(const ValidatorSet &);

    boost::optional<Validator> getByIndex(int index);

    boost::optional<Validator> getByAddress(AddressTm addresstm);

    HexBytes hash() const;

    int64_t getTotalVotingPower();

    unsigned long size();

    Validator *getProposer();

    void incrementAccum(int times);

    bool hasAddress(const HexBytes bytes);

    Validator *findProposer();

    bool add(Validator val);

    // Update updates val and returns true. It returns false if val is not present
// in the set.
    bool update(Validator &val);

    int getIndex(Validator &val);

// Remove deletes the validator with address. It returns the validator removed
// and true. If returns nil and false if validator is not present in the set.
    bool remove(Validator &val);

    // Verify that +2/3 of the set had signed the given signBytes
    void verifyCommit(const std::string &chainID, const BlockID &blockID, height_t height, Commit &commit);
};


#endif //AYETH_VALIDATORSET_H
