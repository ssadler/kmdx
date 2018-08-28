
//
// Created by thpn on 30/07/18.
//

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "../types/Vote.h"
#include "./VoteTest.h"
#include "./VoteSetTest.h"


BOOST_AUTO_TEST_SUITE(tmvotestest)

//    BOOST_AUTO_TEST_CASE (voteTestTm) {
//        VoteTest vt;
//        vt.testVoteSignable();
//        vt.testVoteVerifySignature();
//        vt.testIsVoteTypeValid();
//        vt.testVoteVerify();
//    }

    BOOST_AUTO_TEST_CASE (voteconvTm) {

        Address addresstm(HexBytes::random(10));
        int index = 1;
        height_t height = 5000;
        int round = 3;
        boost::posix_time::ptime t = boost::posix_time::second_clock::local_time();
        BlockID blockID(HexBytes::random(10));

        Vote v(addresstm, index, height, round, t, Vote::allVoteTypes[1], blockID);

        dev::RLPStream stream = v.toRLP();
        dev::RLP r(stream.out());
        Vote o = Vote::fromRLP(r);
        BOOST_CHECK(o.getValidatorAddress() == v.getValidatorAddress());
        BOOST_CHECK(o.getValidatorIndex() == v.getValidatorIndex());
        BOOST_CHECK(o.getHeight() == v.getHeight());
        BOOST_CHECK(o.getRoundNumber() == v.getRoundNumber());
        BOOST_CHECK(o.getTimestamp() == v.getTimestamp());
        BOOST_CHECK(o.getType() == v.getType());
        BOOST_CHECK(o.getBlockID() == v.getBlockID());
    }

    BOOST_AUTO_TEST_CASE (testAddVote) {
        VoteSetTest vst;
        vst.testAddVote();
    }

    BOOST_AUTO_TEST_CASE (test2_3Majority_not_null) {
        VoteSetTest vst;
        vst.test2_3Majority_not_null();
    }

    BOOST_AUTO_TEST_CASE (test2_3Majority) {
        VoteSetTest vst;
        vst.test2_3Majority();
    }

    BOOST_AUTO_TEST_CASE (test2_3MajorityRedux) {
        VoteSetTest vst;
        vst.test2_3MajorityRedux();

    }

    BOOST_AUTO_TEST_CASE (testBadVotes) {
        VoteSetTest vst;
        vst.testBadVotes();

    }

    BOOST_AUTO_TEST_CASE (testConflicts) {
        VoteSetTest vst;
        vst.testConflicts();

    }

    BOOST_AUTO_TEST_CASE (testMakeCommit) {
        VoteSetTest vst;
        vst.testMakeCommit();

    }
BOOST_AUTO_TEST_SUITE_END();

