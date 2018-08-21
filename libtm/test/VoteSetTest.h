//
// Created by thpn on 13/08/18.
//

#ifndef AYETH_VOTESETTEST_H
#define AYETH_VOTESETTEST_H

#include <cstdint>
#include "../types/VoteSet.h"
#include "../types/HexBytes.h"
#include "../types/PrivValidator.h"
#include "common_test.h"

class VoteSetTest {
public:
    VoteSetTest();

    //VoteSet randVoteSet(int64_t height, int round, VoteType _type);
    Vote withValidator(Vote _vote, HexBytes _addr, int _idx);

    Vote withHeight(Vote &_vote, int64_t _height);

    Vote withRound(Vote _vote, int _roundNumber);

    Vote withType(Vote _vote, VoteType _type);

    Vote withBlockHash(Vote _vote, HexBytes _hash);

    void testAddVote();

    void test2_3Majority();

    void test2_3MajorityRedux();

    void testBadVotes();

    void testConflicts();
};


#endif //AYETH_VOTESETTEST_H
