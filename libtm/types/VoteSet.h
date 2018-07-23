//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_VOTESET_H
#define TM_LIGHT_VOTESET_H

#include <string>
#include "Vote.h"

class VoteSet {
//TODO
public:
    bool addVote(Vote v);

    std::string toStringShort();

    bool hasAllVotes() const;

    bool twoThirdMajority(BlockID &bytes);

    bool hasTwoThirdsAny();

    bool hasTwoThirdMajority();

    bool hasAll();
};


#endif //TM_LIGHT_VOTESET_H
