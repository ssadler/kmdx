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
    return Vote(AddressTm(HexBytes("addr")),
                56789, //ValidatorIndex
                12345, //Height
                2, //Round
                boost::posix_time::from_iso_string("20180901T090950"),
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
            << "{chain_id:test_chain_id,type:vote,{block_id:{hash:hash}},height:12345,round:2,timestamp:20180901T090950,type:"
            << Vote::voteTypeToString(VoteTypePrecommit) << "}"; //FIXME hashes
    BOOST_CHECK_EQUAL(expected.str(), signStr);
}

void VoteTest::testVoteVerifySignature() {
    MockPV privVal("abcdefabcdef1234abcdefabcdef1234");
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
    MockPV mockPV("abcdefabcdef1234abcdefabcdef1234");
    PubKey pubKey = mockPV.getPubKey();

    Vote vote = examplePrevote();
    vote.validatorAddress = pubKey.getAddress();

    try {
        vote.verify("test_chain_id", PubKey(PrivKey().getKey()));
    } catch (exception &e) {
        BOOST_CHECK_THROW(throw e, ErrInvalidValidatorAddress);
    }

    try {
        vote.verify("test_chain_id", mockPV.getPubKey()); // ed25519.GenPrivKey().PubKey()
    } catch (exception &e) {
        BOOST_CHECK_THROW(throw e, ErrorInvalidSignature);
    }

}

void VoteTest::voteRlpTest() {
    AddressTm addresstm(HexBytes::random(10));
    int index = 1;
    height_t height = 5000;
    int round = 3;
    boost::posix_time::ptime t = boost::posix_time::second_clock::local_time();
    BlockID blockID(HexBytes::random(9));

    Vote v(addresstm, index, height, round, t, Vote::allVoteTypes[2], blockID);
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

void VoteTest::proposalRlpTest() {

    height_t height = 5000;
    int round = 3;
    boost::posix_time::ptime t = boost::posix_time::second_clock::local_time();
    BlockID blockID(HexBytes("testBlockId"));
    Proposal v(height, round, t, 1, blockID);
    dev::RLPStream stream = v.toRLP();
    dev::RLP r(stream.out());
    Proposal o = Proposal::fromRLP(r);
    BOOST_CHECK(o.height == v.height);
    BOOST_CHECK(o.roundNumber == v.roundNumber);
    BOOST_CHECK(o.timestamp == v.timestamp);
    BOOST_CHECK(o.block == v.block);
    BOOST_CHECK(o.polRound == v.polRound);
    BOOST_CHECK(o.polBlockID == v.polBlockID);
    BOOST_CHECK(o.signature == v.signature);
}

void VoteTest::proposalMessageRlpTest() {
    AddressTm addresstm(HexBytes::random(10));
    height_t height = 5000;
    int round = 3;
    boost::posix_time::ptime t = boost::posix_time::second_clock::local_time();
    BlockID blockID(HexBytes("testBlockId"));
    Proposal p(height, round, t, 1, blockID);
    ProposalMessage v(addresstm, p);
    dev::RLPStream stream = v.toRLP();
    dev::RLP r(stream.out());
    ProposalMessage o = ProposalMessage::fromRLP(r);
    Proposal op = o.getProposal();
    BOOST_CHECK(op.height == p.height);
    BOOST_CHECK(op.roundNumber == p.roundNumber);
    BOOST_CHECK(op.timestamp == p.timestamp);
    BOOST_CHECK(op.block == p.block);
    BOOST_CHECK(op.polRound == p.polRound);
    BOOST_CHECK(op.polBlockID == p.polBlockID);
    BOOST_CHECK(op.signature == p.signature);
    BOOST_CHECK(v.getAddress() == o.getAddress());
}

void VoteTest::blockIdRlpTest() {
    BlockID bid(HexBytes("testBlockId"));
    dev::RLPStream bidstream = bid.toRLP();
    dev::RLP bidrlp(bidstream.out());
    BlockID recovered = BlockID::fromRLP(bidrlp);
    BOOST_CHECK(bid.getBytes() == recovered.getBytes());
    BOOST_CHECK(bid.getHash() == recovered.getHash());
}

