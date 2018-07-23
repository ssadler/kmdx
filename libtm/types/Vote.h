//
// Created by utnso on 12/07/18.
//

#ifndef TM_LIGHT_VOTE_H
#define TM_LIGHT_VOTE_H


#include <ctime>
#include <cstdint>
#include <string>
#include "HexBytes.h"
#include "Signature.h"

enum VoteType {
    VoteTypePrevote, VoteTypePrecommit,
};

std::string voteTypeToString(VoteType type) {
    switch (type) {
        case VoteTypePrevote:
            return "VoteTypePrevote";
        case VoteTypePrecommit:
            return "VoteTypePrecommit";
    }
    throw "undefined exception type";
};

class Vote {
    HexBytes validatorAddress;
    int validatorIndex;
    int64_t height;
    int roundNumber;
    time_t timestamp;
    enum VoteType type;
    BlockID blockID;
    Signature signature; // comes from crypto
public:
    const HexBytes &getValidatorAddress() const;

    int getValidatorIndex() const;

    int64_t getHeight() const;

    int getRoundNumber() const;

    time_t getTimestamp() const;

    VoteType getType() const;

    const BlockID &getBlockID() const;

    const Signature &getSignature() const;
};


#endif //TM_LIGHT_VOTE_H
