//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_HEIGHTVOTESET_H
#define TM_LIGHT_HEIGHTVOTESET_H

#include <boost/optional.hpp>
#include <memory>
#include "VoteSet.h"

class HeightVoteSet {

public :
    HeightVoteSet(const string &basic_string, int64_t i, ValidatorSet set);

    VoteSet getPrevotes(int i);

    boost::optional<VoteSet> getPrecommits(int i);

    /*returns int = polRound & sets blockID */
    int polInfo(BlockID &blockID);

    int64_t addVote(Vote vote, HexBytes bytes);

    void setRoundNumber(int i);

    bool isEmpty();
};


#endif //TM_LIGHT_HEIGHTVOTESET_H
