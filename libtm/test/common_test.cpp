//
// Created by thpn on 04/08/18.
//

#include "common_test.h"

Vote ValidatorStub::signVote(VoteType voteType, bytes hash) {
    Vote vote(index, privValidator.getAddress(), height, round, boost::posix_time::second_clock::local_time(), voteType,
              BlockID(hash));
    privValidator.signVote(chain_ID, vote);
    return vote;
}

//for testing
ConsensusState
common_test::randGenesisState(vector<PrivValidator> privValidators, dev::RLP genesisParams, int numValidators,
                              bool randPower, int64_t minPower) {
    vector<PrivValidator> privVals;
    ValidatorSet validators = randValidatorSet(privVals, numValidators, randPower, minPower);
    ValidatorSet lastValidators = randValidatorSet(privVals, numValidators, randPower, minPower);
    ConsensusConfig config;
    State state(
            genesisParams[0].toString(), //chainID,
            int64_t(0), //_lastBlockHeight
            BlockID(),
            boost::posix_time::from_iso_string(genesisParams[1].toString()), //lastBlockTime
            validators,
            lastValidators,
            int64_t(1), //lastHeightValidatorsChanged
            ConsensusConfig::fromRLP(genesisParams[2]), //configParams;
            int64_t(1), //lastHeightConsensusConfigChanged
            HexBytes(genesisParams[3].toBytes()) //appHash
    );
    randValidatorSet(privValidators, numValidators, randPower, minPower);
    //ConsensusState cs(ConsensusConfig(), state);//, counter.NewCounterApplication(true))
    return ConsensusState(config, state);
}

void common_test::startTestRound(ConsensusState &cs, int64_t height, int round) {
    cs.enterNewRound(height, round);
    cs.startRoutines();
}


// Create proposal block from cs1 but sign it with vs
Proposal common_test::decideProposal(ConsensusState &cs1, ValidatorStub &vs, int64_t height, int round) {
    boost::optional<Block> block = cs1.createProposalBlock();
    if (!block.is_initialized())
        throw Panic("error creating proposal block");
    // Make proposal
    BlockID polBlockID;
    int polRound = cs1.roundState.votes.polInfo(polBlockID);
    Proposal proposal(int64_t(height), round, polRound, polBlockID);
    try {
        vs.signProposal(cs1.state.getChainID(), proposal);
    } catch (Error &e) {
        throw Panic(e.getDescription());
    }
    return proposal;
}

void common_test::addVotes(ConsensusState &cs, std::vector<Vote> votes) {
    for (Vote &vote : votes)
        cs.addToPeerMsgQueue(VoteMessage(PeerID(bytes()), vote));//FIXME slicing
}

void common_test::signAddVotes(ConsensusState &cs, VoteType voteType, bytes hash, vector<ValidatorStub> vss) {
    vector<Vote> votes = signVotes(voteType, hash, vss);
    addVotes(cs, votes);
}

void common_test::validatePrevote(ConsensusState &cs, int round, ValidatorStub privVal, HexBytes blockHash) {
    VoteSet prevotes = cs.roundState.votes.getPrevotes(round);
    boost::optional<Vote> vote = prevotes.getByAddress(privVal.getAddress());
    if (!vote.is_initialized())
        throw Panic("Failed to find prevote from validator");

    if (!blockHash.empty() && !vote.get().getBlockID().getHash().empty()) { //FIXME pasamanos
        std::ostringstream out;
        out << "Expected prevote to be for nil, got %X" << vote.get().getBlockID().getHash().toString();
        throw Panic(out.str());
    }

    if (vote.get().getBlockID().getHash() != blockHash) {
        std::ostringstream out;
        out << "Expected prevote to be for %X, got %X" << blockHash.toString() <<
            vote.get().getBlockID().getHash().toString();
        throw Panic(out.str());


    }
}

void common_test::validateLastPrecommit(ConsensusState &cs, ValidatorStub &privVal, HexBytes blockHash) {
    VoteSet votes = cs.roundState.lastCommit.get();
    boost::optional<Vote> vote = votes.getByAddress(privVal.getAddress());
    if (!vote.is_initialized())
        throw Panic("Failed to find precommit from validator");
    if (vote.get().getBlockID().getHash() != blockHash) {
        std::ostringstream out;
        out << "Expected precommit to be for %X, got %X" << blockHash.toString() <<
            vote.get().getBlockID().getHash().toString();
        throw Panic(out.str());

    }
}

void common_test::validatePrecommit(ConsensusState &cs, int thisRound, int lockRound, ValidatorStub &privVal,
                                    HexBytes votedBlockHash, HexBytes lockedBlockHash) {
    VoteSet precommits = cs.roundState.votes.getPrecommits(thisRound).get();
    boost::optional<Vote> vote = precommits.getByAddress(privVal.getAddress());
    if (!vote.is_initialized())
        throw Panic("Failed to find precommit from validator");
    if (votedBlockHash.empty() && vote.get().getBlockID().getHash().empty())
        throw Panic("Expected precommit to be for NULL");
    if (vote.get().getBlockID().getHash() != votedBlockHash)
        throw Panic("Expected precommit to be for proposal block");
    if (lockedBlockHash.empty() &&
        ((cs.roundState.lockedRoundNumber != lockRound || cs.roundState.lockedBlock != nullptr))) {
        std::ostringstream out;
        out << "Expected to be locked on nil at round %d. Got locked at round %d with block %v" << lockRound <<
            cs.roundState.lockedRoundNumber << cs.roundState.lockedBlock.get()->getHash().toString();
        throw Panic(out.str());
    }
    if (cs.roundState.lockedRoundNumber != lockRound || cs.roundState.lockedBlock.get()->getHash() != lockedBlockHash) {
        std::ostringstream out;
        out << "Expected block to be locked on round %d, got %d. Got locked block %X, expected %X" <<
            lockRound <<
            cs.roundState.lockedRoundNumber << cs.roundState.lockedBlock.get()->getHash().toString()
            << lockedBlockHash.toString();
        throw Panic(out.str());
    }
}

void common_test::validatePrevoteAndPrecommit(ConsensusState &cs, int thisRound, int lockRound, ValidatorStub &privVal,
                                              HexBytes votedBlockHash, HexBytes lockedBlockHash) {
// verify the prevote
    validatePrevote(cs, thisRound, privVal, votedBlockHash);
// verify precommit
    cs.mtx.lock();
    validatePrecommit(cs, thisRound, lockRound, privVal, votedBlockHash, lockedBlockHash);
    cs.mtx.unlock();
}

// genesis
/*void common_test::subscribeToVoter(ConsensusState &cs, Address address) {
chan interface{} {
    voteCh0 := make(chan interface{})
    err := cs.eventBus.Subscribe(context.Background(), testSubscriber, types.EventQueryVote, voteCh0)
    if err != nil{
                panic(fmt.Sprintf("failed to subscribe %s to %v", testSubscriber, types.EventQueryVote))
        }
    voteCh := make(chan interface{})
    go func() {
        for
        v := range voteCh0{
                vote := v.(types.EventDataVote)
                // we only fire for our own votes
                if bytes.Equal(addr, vote.Vote.ValidatorAddress) {
                    voteCh < -v
                }
        }
    }
    ()
    return voteCh//TODO channel
}*/

/*
ValidatorSet common_test::randValidatorSet(vector<PrivValidator> privValidators, int nValidators, bool randPower , int64_t minPower) {
    ValidatorSet validatorSet(vector<Validator>);
    for (int i = 0; i < nValidators; i++) {
        validatorSet.add(newValidatorStub(privValidators[i], i));
        validatorSet.get(i).incrementHeight(); // since cs1 starts at 1
    }
    return validatorSet;
}

PrivValidator common_test::randValidator(bool randPower, int64_t minPower)(boost::optional <Validator> val) {
    PrivValidator privVal = NewMockPV();
    votePower = minPower;
    if (randPower) {
        votePower += int64(cmn.RandUint32())
    }
    val = newValidator(privVal.GetPubKey(), votePower);
    return privVal;
}*/