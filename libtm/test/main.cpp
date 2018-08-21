
//
// Created by thpn on 30/07/18.
//


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LogManager

#include <iostream>
#include <boost/test/included/unit_test.hpp>

#include "../types/Vote.h"
#include "./VoteTest.h"
#include "./VoteSetTest.h"

BOOST_AUTO_TEST_SUITE(tmvotestest)

    BOOST_AUTO_TEST_CASE (randomHexTm) {
        int n = 15;
        HexBytes hb = HexBytes::random(n);
        for (int i1 = 0; i1 < n; ++i1) {
            cout << hb[i1];
        }
        cout << "\n";
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE (voteTestTm) {
        VoteTest vt;
        vt.testVoteSignable();
        vt.testVoteVerifySignature();
        vt.testIsVoteTypeValid();
        vt.testVoteVerify();
    }

    BOOST_AUTO_TEST_CASE (rlpconvTm) {

        dev::RLPStream r(3);
        r << 4;
        std::string str("hola");
        r << str;
//        std::vector<uint8_t> v({1,2,3,4});
        std::vector<uint8_t> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        r << v;
        dev::RLP rlp(r.out());
        auto recoverInt = rlp[0].toInt();
        cout << recoverInt << "\n";
        auto recoverStr = rlp[1].toString();
        cout << recoverStr << "\n";
        auto recoverV = rlp[2].toVector<uint8_t>();
        bytes vec = recoverV;
        cout << recoverV[0] << "\n";
        //cout<<rlp[2].toVector<uint8_t>()
//        BOOST_CHECK_EQUAL(rlp[0].toInt(), 3);
        BOOST_CHECK_EQUAL(rlp[1].toString(), "hola");
//        std::vector<int> recovered(rlp[2].toVector<int>());
//        cout<<"RECOVERED: "<< recovered[0];
        uint8_t recovered = recoverV[2];

        cout << "RECOVERED: " << recovered;

        BOOST_CHECK_EQUAL(recovered, v[2]);
//        BOOST_CHECK_EQUAL(recovered[0], v[0]);

        /* Address address(std::vector<uint8_t>(1234));
         BlockID blockID(std::vector<uint8_t>(1234));
         boost::posix_time::ptime t = boost::posix_time::second_clock::local_time();

         Vote v(address, 1, int64_t(2), 3, t,
                Vote::allVoteTypes[1], blockID, Signature());

         dev::RLP rlp = v.toRLP();
         Vote o = Vote::fromRLP(rlp);
         BOOST_CHECK(o.getHeight() == 2);
         BOOST_CHECK(o.getValidatorAddress() == address);
         BOOST_CHECK(o.getValidatorIndex() == 1);
         BOOST_CHECK(o.getHeight() == int64_t(2));
         BOOST_CHECK(o.getRoundNumber() == 3);
         BOOST_CHECK(o.getTimestamp() == t);
         BOOST_CHECK(o.getType() == Vote::allVoteTypes[1]);
         BOOST_CHECK(o.getBlockID() == blockID);*/
    }

    BOOST_AUTO_TEST_CASE (voteSetTestTm) {
        VoteSetTest vst;
        vst.testAddVote();
    }
BOOST_AUTO_TEST_SUITE_END();

