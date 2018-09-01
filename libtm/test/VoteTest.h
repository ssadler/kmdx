//
// Created by thpn on 10/08/18.
//

#ifndef AYETH_VOTETEST_H
#define AYETH_VOTETEST_H

#include <iostream>
#include <string>
#include <boost/core/ref.hpp>

#include "../types/Vote.h"
#include "../Message/Message.h"
#include "./MockPV.h"
#include "../types/Error.h"


class VoteTest {
private:
public :
    void testVoteSignable();

    void testVoteVerifySignature();

    void testIsVoteTypeValid();

    void testVoteVerify();

    void voteRlpTest();

    void proposalRlpTest();

    void blockIdRlpTest();

    void proposalMessageRlpTest();
};

#endif //AYETH_VOTETEST_H
