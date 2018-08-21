//
// Created by thpn on 13/08/18.
//
#include "VoteSetTest.h"

Vote getVoteProto(int64_t height) {
    int round = 0;
    Vote voteProto(
            Address(HexBytes::random(10)), // NOTE: must fill in // ValidatorAddress:
            0,  // NOTE: must fill in // ValidatorIndex:
            height, // Height:
            round, // Round:
            boost::posix_time::second_clock::local_time(), //Timestamp
            VoteTypePrevote, // Type:
            BlockID(HexBytes::random(10)) // BlockID:
    );
    return voteProto;
}

Vote getVoteProto(int64_t height, Address validatorAddress) {
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

// Convenience: Return new vote with different validator address/index
Vote VoteSetTest::withValidator(Vote _vote, HexBytes _addr, int _idx) {
    Vote vote(_vote);
    vote.validatorAddress = _addr;
    vote.validatorIndex = _idx;
    return vote;
}

// Convenience: Return new vote with different height
Vote VoteSetTest::withHeight(Vote &_vote, int64_t _height) {
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
    vote.blockID.hash = _hash;
    return vote;
}

void VoteSetTest::testAddVote() {
    int round = 0;
    int64_t height = 1;

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
    common_test::signAddVote(val0, voteProto, voteSet);
    BOOST_REQUIRE_MESSAGE(voteSet.getByAddress(val0.getAddress()).is_initialized(),
                          "Expected GetByAddress(val0.Address) to be present");
    BOOST_REQUIRE_MESSAGE(voteSet.getVotes().count(0), "Expected BitArray.getIndex(0) to be true");
    BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(), "There should be no 2/3 majority");
}

void VoteSetTest::test2_3Majority() {
    int round = 0;
    int64_t height = 1;
    int numValidators = 10;
    int64_t votingPower = 1;
    std::vector<PrivValidator> privValidators;
    ValidatorSet validators = common_test::randValidatorSet(privValidators, numValidators, votingPower);
    VoteSet voteSet("test_chain_id", height, round, VoteTypePrevote, validators);

    Vote voteProto = getVoteProto(height);

    // 6 out of 10 voted for NULL.
    for (int i = 0; i < 6; i++) {
        Vote vote = withValidator(voteProto, privValidators[i].getAddress(), i);
        common_test::signAddVote(privValidators[i], vote, voteSet);
    }
    BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(), "There should be no 2/3 majority");


// 7th validator voted for some blockhash
    {
        Vote vote = withValidator(voteProto, privValidators[6].getAddress(), 6);
        HexBytes randHash;
        vote = withBlockHash(vote, randHash);
        common_test::signAddVote(privValidators[6], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(), "There should be no 2/3 majority");
    }

// 8th validator voted for NULL.
    {
        Vote vote = withValidator(voteProto, privValidators[7].getAddress(), 7);
        common_test::signAddVote(privValidators[7], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(voteSet.hasTwoThirdMajority(), "There should be 2/3 majority for NULL");
        BOOST_REQUIRE_MESSAGE(voteSet.twoThirdMajority().get().isEmpty(), "There should be 2/3 majority for NULL");
    }
}

void VoteSetTest::test2_3MajorityRedux() {
    int64_t height = 1;
    int round = 0;
    int numValidators = 100;
    int64_t votingPower = 1;
    std::vector<PrivValidator> privValidators;
    ValidatorSet validators = common_test::randValidatorSet(privValidators, numValidators, votingPower);
    VoteSet voteSet("test_chain_id", height, round, VoteTypePrevote, validators);
    HexBytes blockHash(HexBytes::random(10));
    Vote voteProto = getVoteProto(height);

// 66 out of 100 voted for NULL.
    for (int i = 0; i < 66; i++) {
        Vote vote = withValidator(voteProto, privValidators[i].getAddress(), i);
        common_test::signAddVote(privValidators[i], vote, voteSet);
    }
    BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(), "There should be no 2/3 majority");

// 67th validator voted for NULL
    {
        Vote vote = withValidator(voteProto, privValidators[66].getAddress(), 66);
        HexBytes exptyHash;
        vote = withBlockHash(vote, exptyHash);
        common_test::signAddVote(privValidators[66], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(),
                              "There should be no 2/3 majority: last vote added was NULL");
    }

// 68th validator voted for a different BlockParts PartSetHeader
    {
        Vote vote = withValidator(voteProto, privValidators[67].getAddress(), 67);
//        blockPartsHeader = PartSetHeader{blockPartsTotal, crypto.CRandBytes(32)}
        common_test::signAddVote(privValidators[67], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(),
                              "There should be no 2/3 majority: last vote added had different PartSetHeader Hash");
    }

// 69th validator voted for different BlockParts Total
    {
        Vote vote = withValidator(voteProto, privValidators[68].getAddress(), 68);
//        blockPartsHeader = PartSetHeader{blockPartsTotal + 1, blockPartsHeader.Hash}
        common_test::signAddVote(privValidators[68], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(),
                              "There should be no 2/3 majority: last vote added had different PartSetHeader Total");
    }

// 70th validator voted for different BlockHash
    {
        Vote vote = withValidator(voteProto, privValidators[69].getAddress(), 69);
        HexBytes randomHash(HexBytes::random(10));
        vote = withBlockHash(vote, randomHash);
        common_test::signAddVote(privValidators[69], vote, voteSet);

        BOOST_REQUIRE_MESSAGE(!voteSet.hasTwoThirdMajority(),
                              "There should be no 2/3 majority: last vote added had different BlockHash");
    }

// 71st validator voted for the right BlockHash & BlockPartsHeader
    {
        Vote vote = withValidator(voteProto, privValidators[70].getAddress(), 70);
        common_test::signAddVote(privValidators[70], vote, voteSet);

        BOOST_REQUIRE_MESSAGE(voteSet.twoThirdMajority().get().isEmpty(), "There should be 2/3 majority for NULL");
    }
}

void VoteSetTest::testBadVotes() {
    int round = 0;
    int64_t height = 1;
    int numValidators = 10;
    int64_t votingPower = 1;
    std::vector<PrivValidator> privValidators;
    ValidatorSet validators = common_test::randValidatorSet(privValidators, numValidators, votingPower);
    VoteSet voteSet("test_chain_id", height, round, VoteTypePrevote, validators);
    Vote voteProto = getVoteProto(height);
// val0 votes for NULL.
    {
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        HexBytes randomHash(HexBytes::random(10));
        vote = withBlockHash(vote, randomHash);
        bool added = common_test::signAddVote(privValidators[0], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(added, "Expected VoteSet.Add to succeed");
    }

// val0 votes again for some block.
    {
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        HexBytes randomHash2(HexBytes::random(10));
        vote = withBlockHash(vote, randomHash2);
        bool added = common_test::signAddVote(privValidators[0], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(!added, "Expected VoteSet.Add to fail, conflicting vote.");
    }

// val1 votes on another height
    {
        Vote vote = withValidator(voteProto, privValidators[1].getAddress(), 1);
        vote = withHeight(vote, height + 1);
        bool added = common_test::signAddVote(privValidators[1], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(!added, "Expected VoteSet.Add to fail, wrong height");
    }

// val2 votes on another round
    {
        Vote vote = withValidator(voteProto, privValidators[2].getAddress(), 2);
        vote = withRound(vote, round + 1);
        bool added = common_test::signAddVote(privValidators[2], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(!added, "Expected VoteSet.Add to fail, wrong round");
    }

// val3 votes of another type.
    {
        Vote vote = withValidator(voteProto, privValidators[3].getAddress(), 3);
        vote = withType(vote, VoteTypePrecommit);
        bool added = common_test::signAddVote(privValidators[3], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(!added, "Expected VoteSet.Add to fail, wrong type");
    }
}


VoteSetTest::VoteSetTest() {}
/*
void VoteSetTest::testConflicts() {
    int round = 0;
    int64_t height = 1;
    int numValidators = 4;
    int64_t votingPower = 1;
    std::vector<PrivValidator> privValidators;
    ValidatorSet validators = common_test::randValidatorSet(privValidators, numValidators, votingPower);
    VoteSet voteSet("test_chain_id", height, round, VoteTypePrevote, validators);
    HexBytes blockHash1(HexBytes::random(10));
    HexBytes blockHash2(HexBytes::random(10));

    Vote voteProto = getVoteProto(height);

// val0 votes for NULL.
    {
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        bool added = common_test::signAddVote(privValidators[0], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(added, "Expected VoteSet.Add to succeed");
    }

// val0 votes again for blockHash1.
    {
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        vote = withBlockHash(vote, blockHash1);
        bool added = common_test::signAddVote(privValidators[0], vote, voteSet);
        BOOST_REQUIRE_MESSAGE(!added, "Expected VoteSet.Add to fail, conflicting vote.");
//        if err == NULL {
//            throw Error("Expected VoteSet.Add to return error, conflicting vote.", __FILE__, __LINE__);
//    } //FIXME boost_check no exceptions thrown

// start tracking blockHash1
        voteSet.setPeerMaj23("peerA", BlockID(blockHash1));
    }

// val0 votes again for blockHash1.
    {
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        vote = withBlockHash(vote, blockHash1);
        bool added = common_test::signAddVote(privValidators[0], vote, voteSet);
        if (!added) {
            throw Error("Expected VoteSet.Add to succeed, called SetPeerMaj23().", __FILE__, __LINE__);
        }
        if err == NULL
        {
            throw Error("Expected VoteSet.Add to return error, conflicting vote.")
        }
    }

// attempt tracking blockHash2, should fail because already set for peerA.
    voteSet.setPeerMaj23("peerA", BlockID(blockHash2));
            }

// val0 votes again for blockHash1.
    {
        Vote vote = withValidator(voteProto, privValidators[0].getAddress(), 0);
        vote = withBlockHash(vote, blockHash2);
        bool added = common_test::signAddVote(privValidators[0], vote, voteSet);
        if added{
                    throw Error("Expected VoteSet.Add to fail, duplicate SetPeerMaj23() from peerA", __FILE__, __LINE__);
            }
        if err == NULL
        {
            throw Error("Expected VoteSet.Add to return error, conflicting vote.", __FILE__, __LINE__);
        }
    }

// val1 votes for blockHash1.
    {
        Vote vote = withValidator(voteProto, privValidators[1].getAddress(), 1);
        vote = withBlockHash(vote, blockHash1);
        bool added = common_test::signAddVote(privValidators[1], vote, voteSet);
        if (!added) {
            throw Error("Expected VoteSet.Add to succeed", __FILE__, __LINE__);
        }
    }

// check
    if (voteSet.hasTwoThirdsMajority())
    {
        throw Error("We shouldn't have 2/3 majority yet", __FILE__, __LINE__);
    }

    if (voteSet.hasTwoThirdsAny())
    {
        throw Error("We shouldn't have 2/3 if any votes yet", __FILE__, __LINE__);
    }

// val2 votes for blockHash2.
    {
        Vote vote = withValidator(voteProto, privValidators[2].getAddress(), 2);
        vote = withBlockHash(vote, blockHash2);
        bool added = common_test::signAddVote(privValidators[2], vote, voteSet);
        if (!added) {
            throw Error("Expected VoteSet.Add to succeed", __FILE__, __LINE__);
        }
    }

// check
    if (voteSet.hasTwoThirdsMajority())
    {
        throw Error("We shouldn't have 2/3 majority yet", __FILE__, __LINE__);
    }

    if (voteSet.hasTwoThirdsAny())
    {
        throw Error("We should have 2/3 if any votes", __FILE__, __LINE__);
    }

// now attempt tracking blockHash1
    voteSet.setPeerMaj23("peerB", BlockID{
            blockHash1, PartSetHeader{
            }
    })

// val2 votes for blockHash1.
    {
        Vote vote = withValidator(voteProto, privValidators[2].getAddress(), 2);
        vote = withBlockHash(vote, blockHash1);
        bool added = common_test::signAddVote(privValidators[2], vote, voteSet);
        if (!added) {
            throw Error("Expected VoteSet.Add to succeed", __FILE__, __LINE__);
        }
        if err == NULL
        {
            throw Error("Expected VoteSet.Add to return error, conflicting vote", __FILE__, __LINE__);
        }
    }

// check
    if (!voteSet.hasTwoThirdsMajority()) {
        throw Error("We should have 2/3 majority for blockHash1", __FILE__, __LINE__);
    }

    blockIDMaj23,
            _ = voteSet.twoThirdMajority()
    if !bytes.
                Equal(blockIDMaj23
                              .Hash, blockHash1)
    {
        throw Error("Got the wrong 2/3 majority blockhash", __FILE__, __LINE__);
    }
    if (voteSet.hasTwoThirdsAny())
    {
        throw Error("We should have 2/3 if any votes", __FILE__, __LINE__);
    }

}

func TestMakeCommit(t *testing

.T) {
height,
round = int64_t(1),
0
voteSet, _,
privValidators = randVoteSet(height, round, VoteTypePrecommit, 10, 1)
blockHash,
blockPartsHeader = crypto.CRandBytes(32), PartSetHeader{123, crypto.CRandBytes(32)}

Vote voteProto = getVoteProto(height);

// 6 out of 10 voted for some block.
for
i = 0;
i < 6; i++ {
vote = withValidator(voteProto, privValidators[i].getAddress(), i)
_,
err = common_test::signAddVote(privValidators[i], vote, voteSet)
if err != NULL {
t.
Error(err)
}
}

// MakeCommit should fail.
tst.
AssertPanics(t,
"Doesn't have +2/3 majority",

func() { voteSet.MakeCommit() }

)

// 7th voted for some other block.
{
vote = withValidator(voteProto, privValidators[6].getAddress(), 6)
vote = withBlockHash(vote, std::rand())
vote = withBlockPartsHeader(vote, PartSetHeader{123, std::rand()})

_,
err = common_test::signAddVote(privValidators[6], vote, voteSet)
if err != NULL {
t.
Error(err)
}
}

// The 8th voted like everyone else.
{
vote = withValidator(voteProto, privValidators[7].getAddress(), 7)
_,
err = common_test::signAddVote(privValidators[7], vote, voteSet)
if err != NULL {
t.
Error(err)
}
}

commit = voteSet.MakeCommit()

// Commit should have 10 elements
if
len(commit
.Precommits) != 10 {
throw Error("Commit Precommits should have the same number of precommits as validators", __FILE__, __LINE__)
}

// Ensure that Commit precommits are ordered.
if
err = commit.ValidateBasic();
err != NULL {
throw Error("Error in Commit.ValidateBasic(): %v", err, __FILE__, __LINE__)
}

}
*/
