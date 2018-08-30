//
// Created by thpn on 10/08/18.
//


#include "VoteTest.h"

Vote exampleVote(VoteType type);

Vote examplePrevote() {
    return exampleVote(VoteTypePrevote);
}

Vote examplePrecommit() {
    return exampleVote(VoteTypePrecommit);
}

Vote exampleVote(VoteType type) {
/*var stamp, err = time.Parse(TimeFormat, "2017-12-25T03:00:01.234Z")
if err != nil {
panic(err)
}*/

//    Vote(const Address address, int _validatorIndex, height_t _height, int _roundNumber,
//    const boost::posix_time::ptime &_timestamp, VoteType _type, const BlockID _blockID,
//    const Signature _signature);
//

    return Vote(AddressTm(HexBytes("addr")),
                56789, //ValidatorIndex
                12345, //Height
                2, //Round
                boost::posix_time::second_clock::local_time(), //Timestamp
                type, //Type
                BlockID(HexBytes("hash"))
    );
}

void VoteTest::testVoteSignable() {
    Vote vote = examplePrecommit();
    HexBytes signBytes = vote.signBytes("test_chain_id");
    std::string signStr = signBytes.toString();
    ostringstream expected;
    expected
            << "{chain_id:test_chain_id,type:vote,block_id:{hash:hash},height:12345,round:2,timestamp:2017-12-25T03:00:01.234Z,type:"
            << Vote::voteTypeToString(VoteTypePrecommit); //FIXME hashes
    BOOST_CHECK_EQUAL(expected.str(), signStr);
}

void VoteTest::testVoteVerifySignature() {
    PrivKey privKey(HexBytes::random(10), HexBytes::random(10)); //TODO ed25519.GenPrivKey()
    MockPV privVal(privKey);
    PubKey pubKey = privVal.getPubKey();
    Vote vote = examplePrecommit();
    HexBytes signBytes = vote.signBytes("test_chain_id");

    // sign it
    privVal.signVote("test_chain_id", vote);
    //  require.NoError(t, err)

    // verify the same vote
    bool valid = pubKey.verifyBytes(vote.signBytes("test_chain_id"), vote.getSignature());
    BOOST_REQUIRE(valid);

// serialize, deserialize and verify again....
/*precommit = new(Vote)
bs, err := cdc.MarshalBinary(vote)
require.NoError(t, err)
err = cdc.UnmarshalBinary(bs, &precommit)
require.NoError(t, err)*/

// verify the transmitted vote
/*HexBytes newSignBytes = precommit.SignBytes("test_chain_id");
BOOST_CHECK(signBytes == newSignBytes);
bool valid2 = pubKey.verifyBytes(newSignBytes, precommit.Signature);
    BOOST_REQUIRE(valid2);*/
}

void VoteTest::testIsVoteTypeValid() {
    VoteType voteTypes[2] = {VoteTypePrevote, VoteTypePrecommit};
    std::string strings[2] = {"VoteTypePrevote", "VoteTypePrecommit"};
    for (int i = 0; i < 2; i++) {
        BOOST_CHECK_EQUAL(strings[i], Vote::voteTypeToString(voteTypes[i]));
    }
}

void VoteTest::testVoteVerify() {
    PrivKey privKey(HexBytes::random(10), HexBytes::random(10));
    MockPV mockPV(privKey);
    PubKey pubKey = mockPV.getPubKey();

    Vote vote = examplePrevote();
    vote.validatorAddress = pubKey.getAddress();

    try {
        vote.verify("test_chain_id", PubKey(HexBytes::random(10)));
    } catch (exception &e) {
        BOOST_CHECK_THROW(throw e, ErrInvalidValidatorAddress);
    }

    try {
        vote.verify("test_chain_id", mockPV.getPubKey()); // ed25519.GenPrivKey().PubKey()
    } catch (exception &e) {
        BOOST_CHECK_THROW(throw e, ErrorInvalidSignature);
    }

}
