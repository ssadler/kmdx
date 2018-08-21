//
// Created by thpn on 10/08/18.
//

#ifndef AYETH_VOTETEST_H
#define AYETH_VOTETEST_H

#include <iostream>
#include <string>
#include <boost/core/ref.hpp>

#include "../types/Vote.h"
#include "./MockPV.h"
#include "../types/Error.h"


class VoteTest {
private:
    bool bleh;
public :
    void testVoteSignable();

    void testVoteVerifySignature();

    void testIsVoteTypeValid();

    void testVoteVerify();
};

#endif //AYETH_VOTETEST_H
