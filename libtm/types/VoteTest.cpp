//
// Created by thpn on 30/07/18.
//

#include "VoteTest.h"
#include "Vote.h"
#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE example

#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(votes)

    BOOST_AUTO_TEST_CASE (maruo) {
        BOOST_CHECK(true);
        /* PeerID peerID(std::vector<uint8_t >(1234));
         BlockID blockID (std::vector<uint8_t >(1234));
         boost::posix_time::ptime t = boost::posix_time::second_clock::local_time();

         Vote v(peerID, 1, int64_t(2), 3, t,
                Vote::allVoteTypes[1], blockID, Signature());

         dev::RLP rlp = v.toRLP();
         Vote o = Vote::fromRLP(rlp);
         BOOST_CHECK( o.getHeight() == 2 );
         BOOST_CHECK(o.getValidatorAddress() == peerID );
         BOOST_CHECK(o.getValidatorIndex() == 1 );
         BOOST_CHECK(o.getHeight() == int64_t(2));
         BOOST_CHECK(o.getRoundNumber() == 3 );
         BOOST_CHECK(o.getTimestamp() == t);
         BOOST_CHECK(o.getType() == Vote::allVoteTypes[1] );
         BOOST_CHECK(o.getBlockID() == blockID );*/
    }

BOOST_AUTO_TEST_SUITE_END();
