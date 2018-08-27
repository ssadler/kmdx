#include "VoteSetTest.h"

//
// Created by thpn on 13/08/18.
//
VoteSetTest::VoteSetTest() {}

Vote getVoteProto(height_t height) {
    int round = 0;
    Vote voteProto(
            Address(HexBytes::random(10)), // NOTE: must fill in // ValidatorAddress:
            0,  // NOTE: must fill in // ValidatorIndex:
            height, // Height:
            round, // Round:
            boost::posix_time::second_clock::local_time(), //Timestamp
            VoteTypePrevote, // Type:
            BlockID() // BlockID:
    );
    return voteProto;
}

void VoteSetTest::privValSignAdd(const vector<PrivValidator> &privValidators, VoteSet &voteSet, const Vote &voteProto,
                                 int i, boost::optional<Vote> &conflicting) {
    Vote vote = withValidator(voteProto, privValidators[i].getAddress(), i);
    common_test::signAddVote(privValidators[i], vote, voteSet, conflicting);
}

Vote getVoteProto(height_t height, Address validatorAddress) {
    int round = 0;
    Vote voteProto(
            validatorAddress, // NOTE: must fill in // ValidatorAddress:
            0,  // NOTE: must fill in // ValidatorIndex:
            height, // Height:
            round, // Round:
            boost::posix_time::second_clock::local_time(), //Timestamp
            VoteTypePrevote, // Type:
            BlockID(HexBytes::random(10)) // BlockID:
    );
    return voteProto;
}

// Convenience: Return new vote with different validator addresstm/index
Vote VoteSetTest::withValidator(Vote _vote, HexBytes _addr, int _idx) {
    Vote vote(_vote);
    vote.validatorAddress = _addr;
    vote.validatorIndex = _idx;
    return vote;
}

// Convenience: Return new vote with different height
Vote VoteSetTest::withHeight(Vote &_vote, height_t _height) {
    Vote vote(_vote);
    vote.height = _height;
    return vote;
}

// Convenience: Return new vote with different round
Vote VoteSetTest::withRound(Vote _vote, int _roundNumber) {
    Vote vote(_vote);
    vote.roundNumber = _roundNumber;
    return vote;
}

// Convenience: Return new vote with different type
Vote VoteSetTest::withType(Vote _vote, VoteType _type) {
    Vote vote(_vote);
    vote.type = _type;
    return vote;
}

// Convenience: Return new vote with different blockHash
Vote VoteSetTest::withBlockHash(Vote _vote, HexBytes _hash) {
    Vote vote(_vote);
    vote.blockID.bytes = _hash;
    vote.blockID.hash = _hash;
    return vote;
}

void VoteSetTest::testAddVote() {
    int round = 0;
    height_t height = 1;

    int numValidators = 10;
    int64_t votingPower = 1;
    std::vector<PrivValidator> privValidators;
    ValidatorSet validators = common_test::randValidatorSet(privValidators, numValidators, votingPower);
    VoteSet voteSet("test_chain_id", height, round, VoteTypePrevote, validators);
    PrivValidator val0 = privValidators[0];

// t.Logf(">> %v", voteSet)
    BOOST_REQUIRE_MESSAGE(!voteSet.getByAddress(val0.getAddress()).is_initialized(),
                          "Expected GetByAddress(val0.Address) to be NULL");
    BOOST_REQUIRE_MESSAGE(!voteSet.getVotes().count(0), "Expected BitArray.getIndex(0) to be false");
    BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(), "There should be no 2/3 majority");

    Vote voteProto = getVoteProto(height, val0.getAddress());
    boost::optional<Vote> conflicting;
    common_test::signAddVote(val0, voteProto, voteSet, conflicting);
    BOOST_REQUIRE_MESSAGE(voteSet.getByAddress(val0.getAddress()).is_initialized(),
                          "Expected GetByAddress(val0.Address) to be present");
    BOOST_REQUIRE_MESSAGE(voteSet.getVotes().count(0), "Expected BitArray.getIndex(0) to be true");
    BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(), "There should be no 2/3 majority");
}

void VoteSetTest::test2_3Majority_not_null() {
    int round = 0;
    height_t height = 1;
    int numValidators = 10;
    int64_t votingPower = 1;
    std::vector<PrivValidator> privValidators;
    ValidatorSet validators = common_test::randValidatorSet(privValidators, numValidators, votingPower);
    VoteSet voteSet("test_chain_id", height, round, VoteTypePrevote, validators);

    Vote voteProto = getVoteProto(height);
    boost::optional<Vote> conflicting;
    // 6 out of 10 voted for NULL.

    for (int i = 0; i < 7; i++) {
        Vote vote = withValidator(voteProto, privValidators[i].getAddress(), i);
        vote = withBlockHash(vote, HexBytes::random(i));
        common_test::signAddVote(privValidators[i], vote, voteSet, conflicting);
        conflicting.reset();
    }
    BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(), "There should be no 2/3 majority");
}

void VoteSetTest::test2_3Majority() {
    int round = 0;
    height_t height = 1;
    int numValidators = 10;
    int64_t votingPower = 1;
    std::vector<PrivValidator> privValidators;
    ValidatorSet validators = common_test::randValidatorSet(privValidators, numValidators, votingPower);
    VoteSet voteSet("test_chain_id", height, round, VoteTypePrevote, validators);

    Vote voteProto = getVoteProto(height);
    boost::optional<Vote> conflicting;
    // 6 out of 10 voted for NULL.
    for (int i = 0; i < 6; i++) {
        Vote vote = withValidator(voteProto, privValidators[i].getAddress(), i);
        common_test::signAddVote(privValidators[i], vote, voteSet, conflicting);
    }
    BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(), "There should be no 2/3 majority");


// 7th validator voted for some blockhash
    {
        Vote vote = withValidator(voteProto, privValidators[6].getAddress(), 6);
        HexBytes randHash(HexBytes::random(10));
        vote = withBlockHash(vote, randHash);
        common_test::signAddVote(privValidators[6], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(), "There should be no 2/3 majority");
    }

// 8th validator voted for NULL.
    {
        Vote vote = withValidator(voteProto, privValidators[7].getAddress(), 7);
        common_test::signAddVote(privValidators[7], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(voteSet.hasTwoThirdMajority(), "There should be 2/3 majority");
        BOOST_REQUIRE_MESSAGE(voteSet.twoThirdMajority()->isEmpty(), "The 2/3 majority should be for NULL");
    }
}

void VoteSetTest::test2_3MajorityRedux() {
    height_t height = 1;
    int round = 0;
    int numValidators = 100;
    int64_t votingPower = 1;
    std::vector<PrivValidator> privValidators;
    ValidatorSet validators = common_test::randValidatorSet(privValidators, numValidators, votingPower);
    VoteSet voteSet("test_chain_id", height, round, VoteTypePrevote, validators);
    HexBytes blockHash(HexBytes::random(10));
//    Vote voteProto = getVoteProto(height);
    boost::optional<Vote> conflicting;
// 66 out of 100 voted for NULL.
    for (int i = 0; i < 66; i++) {
        privValSignAdd(privValidators, voteSet, getVoteProto(height), i, conflicting);
    }
    BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(), "There should be no 2/3 majority");

    //These validations are for Block properties we are not considering at the moment.
/*// 67th validator voted for NULL
    {
        Vote vote = withValidator(voteProto, privValidators[67].getAddress(), 67);
        HexBytes exptyHash;
        vote = withBlockHash(vote, exptyHash);
        privValSignAdd(privValidators, voteSet, voteProto, 67);
        BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(),
                              "There should be no 2/3 majority: last vote added was NULL");
    }

// 68th validator voted for a different BlockParts PartSetHeader
    {
        Vote vote = withValidator(voteProto, privValidators[67].getAddress(), 67);
//        blockPartsHeader = PartSetHeader{blockPartsTotal, crypto.CRandBytes(32)}
        common_test::signAddVote(privValidators[67], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(),
                              "There should be no 2/3 majority: last vote added had different PartSetHeader Hash");
    }

// 69th validator voted for different BlockParts Total
    {
        Vote vote = withValidator(voteProto, privValidators[68].getAddress(), 68);
//        blockPartsHeader = PartSetHeader{blockPartsTotal + 1, blockPartsHeader.Hash}
        common_test::signAddVote(privValidators[68], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(),
                              "There should be no 2/3 majority: last vote added had different PartSetHeader Total");
    }*/

// 70th validator voted for different BlockHash
    {
        Vote vote = getVoteProto(height);
        HexBytes randomHash(HexBytes::random(10));
        vote = withBlockHash(vote, randomHash);
        privValSignAdd(privValidators, voteSet, vote, 67, conflicting);
        BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(),
                              "There should be no 2/3 majority: last vote added had different BlockHash");
    }

// 71st validator voted for the right BlockHash & BlockPartsHeader
    {
        privValSignAdd(privValidators, voteSet, getVoteProto(height), 68, conflicting);
        BOOST_REQUIRE_MESSAGE(!voteSet.twoThirdMajority().is_initialized() || voteSet.twoThirdMajority()->isEmpty(),
                              "There should be 2/3 majority for NULL");
    }
}

void VoteSetTest::testBadVotes() {
    int round = 0;
    height_t height = 1;
    int numValidators = 10;
    int64_t votingPower = 1;
    std::vector<PrivValidator> privValidators;
    ValidatorSet validators = common_test::randValidatorSet(privValidators, numValidators, votingPower);
    VoteSet voteSet("test_chain_id", height, round, VoteTypePrevote, validators);
    Vote voteProto = getVoteProto(height);
    boost::optional<Vote> conflicting;
// val0 votes for NULL.
    {
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        HexBytes randomHash(HexBytes::random(10));
        vote = withBlockHash(vote, randomHash);
        AddVoteResult res = common_test::signAddVote(privValidators[0], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(res.isAdded(), "Expected VoteSet.Add to succeed");
    }

// val0 votes again for some block.
    {
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        HexBytes randomHash2(HexBytes::random(10));
        vote = withBlockHash(vote, randomHash2);
        AddVoteResult res = common_test::signAddVote(privValidators[0], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(!res.isAdded(), "Expected VoteSet.Add to fail, conflicting vote.");
    }

// val1 votes on another height
    {
        Vote vote = withValidator(voteProto, privValidators[1].getAddress(), 1);
        vote = withHeight(vote, height + 1);
        //AddVoteResult res = common_test::signAddVote(privValidators[1], vote, voteSet, conflicting);
        AddVoteResult res = common_test::signAddVote(privValidators[0], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(!res.isAdded(), "Expected VoteSet.Add to fail, wrong height");
    }

// val2 votes on another round
    {
        Vote vote = withValidator(voteProto, privValidators[2].getAddress(), 2);
        vote = withRound(vote, round + 1);
        AddVoteResult res = common_test::signAddVote(privValidators[0], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(!res.isAdded(), "Expected VoteSet.Add to fail, wrong round");
    }

// val3 votes of another type.
    {
        Vote vote = withValidator(voteProto, privValidators[3].getAddress(), 3);
        vote = withType(vote, VoteTypePrecommit);
        AddVoteResult res = common_test::signAddVote(privValidators[0], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(!res.isAdded(), "Expected VoteSet.Add to fail, wrong type");
    }
}


void VoteSetTest::testConflicts() {
    int round = 0;
    height_t height = 1;
    int numValidators = 4;
    int64_t votingPower = 1;
    std::vector<PrivValidator> privValidators;
    ValidatorSet validators = common_test::randValidatorSet(privValidators, numValidators, votingPower);
    VoteSet voteSet("test_chain_id", height, round, VoteTypePrevote, validators);
    HexBytes blockHash1(HexBytes::random(10));
    HexBytes blockHash2(HexBytes::random(9));
    boost::optional<Vote> conflicting;
    Vote voteProto = getVoteProto(height);

// val0 votes for NULL.
    {
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        AddVoteResult res = common_test::signAddVote(privValidators[0], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(res.isAdded(), "Expected VoteSet.Add to succeed");
    }

// val0 votes again for blockHash1.
    {
        conflicting.reset();
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        vote = withBlockHash(vote, blockHash1);
        AddVoteResult res = common_test::signAddVote(privValidators[0], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(!res.isAdded(), "Expected VoteSet.Add to fail, conflicting vote.");
        BOOST_REQUIRE_MESSAGE(res.isError(), "Expected VoteSet.Add to return error, conflicting vote.");

// start tracking blockHash1
        voteSet.setPeerMaj23(P2PID("peerA"), BlockID(blockHash1));
    }
// val0 votes again for blockHash1.
    {
        conflicting.reset();
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        vote = withBlockHash(vote, blockHash1);
        AddVoteResult res = common_test::signAddVote(privValidators[0], vote, voteSet, conflicting);
        BOOST_CHECK_MESSAGE(res.isAdded(), "Expected VoteSet.Add to succeed, called SetPeerMaj23().");
        BOOST_REQUIRE_MESSAGE(res.isError(), "Expected VoteSet.Add to return error, conflicting vote.");
    }

// attempt tracking blockHash2, should fail because already set for peerA.
    AddVoteResult res = voteSet.setPeerMaj23(P2PID("peerA"), BlockID(blockHash2));
    BOOST_REQUIRE_MESSAGE(res.isError(), "Expected VoteSet.Add to return error, conflicting vote.");
// val0 votes again for blockHash1.
    {
        conflicting.reset();
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        vote = withBlockHash(vote, blockHash2);
        AddVoteResult res = common_test::signAddVote(privValidators[0], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(!res.isAdded(), "Expected VoteSet.Add to fail, duplicate SetPeerMaj23() from peerA");
        BOOST_REQUIRE_MESSAGE(res.isError(), "Expected VoteSet.Add to return error, conflicting vote.");
    }

// val1 votes for blockHash1.
    {
        conflicting.reset();
        Vote vote = withValidator(voteProto, privValidators[1].getAddress(), 1);
        vote = withBlockHash(vote, blockHash1);
        AddVoteResult res = common_test::signAddVote(privValidators[1], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(res.isAdded(), "Expected VoteSet.Add to succeed");
    }

// check
    BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(), "We shouldn't have 2/3 majority yet");
    BOOST_CHECK_MESSAGE(!voteSet.hasTwoThirdsAny(), "We shouldn't have 2/3 if any votes yet");

// val2 votes for blockHash2.
    {
        conflicting.reset();
        Vote vote = withValidator(voteProto, privValidators[2].getAddress(), 2);
        vote = withBlockHash(vote, blockHash2);
        AddVoteResult res = common_test::signAddVote(privValidators[2], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(res.isAdded(), "Expected VoteSet.Add to succeed");
    }


// check
    BOOST_CHECK_MESSAGE(!voteSet.hasTwoThirdMajority(), "We shouldn't have 2/3 majority yet");
    BOOST_REQUIRE_MESSAGE(voteSet.hasTwoThirdsAny(), "We should have 2/3");


// now attempt tracking blockHash1
    voteSet.setPeerMaj23(P2PID("peerB"), BlockID(blockHash1)
    );

// val2 votes for blockHash1.
    {
        conflicting.reset();
        Vote vote = withValidator(voteProto, privValidators[2].getAddress(), 2);
        vote = withBlockHash(vote, blockHash1);
        AddVoteResult res = common_test::signAddVote(privValidators[2], vote, voteSet, conflicting);
        BOOST_REQUIRE_MESSAGE(res.isAdded(), "Expected VoteSet.Add to succeed");
        BOOST_REQUIRE_MESSAGE(res.isError(), "Expected VoteSet.Add to return error, conflicting vote");
    }

// check

    BOOST_REQUIRE_MESSAGE(voteSet.hasTwoThirdMajority(), "We should have 2/3 majority for blockHash1");

    boost::optional<BlockID> blockIDMaj23 = voteSet.twoThirdMajority();
    BOOST_REQUIRE_MESSAGE(blockIDMaj23.is_initialized() && blockIDMaj23->getHash() == blockHash1,
                          "Got the wrong 2/3 majority blockhash");
    BOOST_REQUIRE_MESSAGE(voteSet.hasTwoThirdsAny(), "We should have 2/3 if any votes");
}

void VoteSetTest::testMakeCommit() {
    int round = 0;
    height_t height = 1;
    int numValidators = 10;
    int64_t votingPower = 1;
    std::vector<PrivValidator> privValidators;
    ValidatorSet validators = common_test::randValidatorSet(privValidators, numValidators, votingPower);
    VoteSet voteSet("test_chain_id", height, round, VoteTypePrevote, validators);
    HexBytes hash = HexBytes::random(10);
    boost::optional<Vote> conflicting;
    Vote voteProto = getVoteProto(height);

// 6 out of 10 voted for some block.
    for (int i = 0; i < 6; i++) {
        privValSignAdd(privValidators, voteSet, getVoteProto(height), i, conflicting);
    }
// MakeCommit should fail.
    BOOST_CHECK_THROW(voteSet.makeCommit(), Error);// "Doesn't have +2/3 majority",

}
// 7th voted for some other block.
/*{
    Vote vote = withValidator(voteProto, privValidators[6].getAddress(), 6);
    vote = withBlockHash(vote, std::rand());
    vote = withBlockPartsHeader(vote, PartSetHeader{123, std::rand()});

    _,
            err = common_test::signAddVote(privValidators[6], vote, voteSet)
    if err != NULL
    {
        t.
                Error(err)
    }
}

// The 8th voted like everyone else.
{
    vote = withValidator(voteProto, privValidators[7].getAddress(), 7)
    _,
            err = common_test::signAddVote(privValidators[7], vote, voteSet)
    if err != NULL
    {
        t.
                Error(err)
    }
}

commit = voteSet.MakeCommit()

// Commit should have 10 elements
if
    len(commit
                .Precommits) != 10
{
    throw Error("Commit Precommits should have the same number of precommits as validators", __FILE__, __LINE__)
}

// Ensure that Commit precommits are ordered.
if
    err = commit.ValidateBasic();
err != NULL
{
    throw Error("Error in Commit.ValidateBasic(): %v", err, __FILE__, __LINE__)
}

}

*/
