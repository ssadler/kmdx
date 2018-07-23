//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_HEIGHTVOTESET_H
#define TM_LIGHT_HEIGHTVOTESET_H


#include "VoteSet.h"

class HeightVoteSet {
//TODO
public :
    VoteSet getPrevotes(int i);

    VoteSet getPrecommits(int i);

    int polInfo(BlockID &blockID);
};


#endif //TM_LIGHT_HEIGHTVOTESET_H
