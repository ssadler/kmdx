//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_HEIGHTVOTESET_H
#define TM_LIGHT_HEIGHTVOTESET_H


#include <memory>
#include "VoteSet.h"

class HeightVoteSet {

public :
    HeightVoteSet(const string &basic_string, int64_t i, ValidatorSet set);

    VoteSet getPrevotes(int i);

    shared_ptr<VoteSet> getPrecommits(int i);

    int polInfo(BlockID &blockID);

    int64_t addVote(Vote vote, HexBytes bytes);

    void setRoundNumber(int i);

    bool isEmpty();
};


#endif //TM_LIGHT_HEIGHTVOTESET_H
