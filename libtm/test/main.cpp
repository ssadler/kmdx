
//
// Created by thpn on 30/07/18.
//

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "../types/Vote.h"
#include "./VoteTest.h"
#include "./VoteSetTest.h"
#include "./ValidatorSetTest.h"

BOOST_AUTO_TEST_SUITE(rlpConversionsTestTm)

    BOOST_AUTO_TEST_CASE (voteRlpTest) {
        VoteTest t;
        t.voteRlpTest();

    }

    BOOST_AUTO_TEST_CASE (blockIdRlpTest) {
        VoteTest t;
        t.blockIdRlpTest();

    }

    BOOST_AUTO_TEST_CASE (proposalRlpTest) {
        VoteTest t;
        t.proposalRlpTest();

    }

    BOOST_AUTO_TEST_CASE (proposalMessageRlpTest) {
        VoteTest t;
        t.proposalMessageRlpTest();

    }

BOOST_AUTO_TEST_SUITE_END();

BOOST_AUTO_TEST_SUITE(tmvotestest)

    BOOST_AUTO_TEST_CASE (voteTestTm) {
        VoteTest vt;
//        vt.testVoteSignable();
        vt.testVoteVerifySignature();
        vt.testIsVoteTypeValid();
        vt.testVoteVerify();
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

BOOST_AUTO_TEST_SUITE(validatorSetTestSuite)

    BOOST_AUTO_TEST_CASE (testValidatorSetBasic) {
        ValidatorSetTest valst;
        valst.testValidatorSetBasic();
    }

    BOOST_AUTO_TEST_CASE (testProposerSelection1) {
        ValidatorSetTest valst;
        valst.testProposerSelection1();
    }

    BOOST_AUTO_TEST_CASE (testProposerSelectionProportional) {
        ValidatorSetTest valst;
        valst.testProposerSelectionProportional();
    }

    BOOST_AUTO_TEST_CASE (testProposerSelectionTwice) {
        ValidatorSetTest valst;
        valst.testProposerSelectionTwice();
    }

    BOOST_AUTO_TEST_CASE (testProposerSelectionNotTwice) {
        ValidatorSetTest valst;
        valst.testProposerSelectionNotTwice();
    }

    BOOST_AUTO_TEST_CASE (testProposerSelectionByAddress) {
        ValidatorSetTest valst;
        valst.testProposerSelectionByAddress();
    }

    BOOST_AUTO_TEST_CASE (testProposerSelection3) {
        ValidatorSetTest valst;
        valst.testProposerSelection3();
    }

    BOOST_AUTO_TEST_CASE (testValidatorSetVerifyCommit) {
        ValidatorSetTest valst;
        valst.testValidatorSetVerifyCommit();
    }

BOOST_AUTO_TEST_SUITE_END();
