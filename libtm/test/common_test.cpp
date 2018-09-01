//
// Created by thpn on 04/08/18.
//

#include "common_test.h"

std::string chain_ID;

Vote ValidatorStub::signVote(VoteType voteType, HexBytes hash) {
    Vote vote(privValidator.getAddress(), index, height, round, boost::posix_time::second_clock::local_time(), voteType,
              BlockID(hash));
    privValidator.signVote(chain_ID, vote);
    return vote;
}

ValidatorSet
common_test::randValidatorSet(std::vector<PrivValidator> &privValidators, int numValidators, int64_t votingPower) {
    return randValidatorSet(privValidators, numValidators, false, votingPower);
}

ValidatorSet
common_test::randValidatorSet(std::vector<PrivValidator> &privValidators, int numValidators, bool randPower,
                              int64_t votingPower) {
    std::vector<Validator> vals;
    for (int i = 0; i < numValidators; i++) {
        ValidatorStub vStub = common_test::randValidator(randPower, votingPower);
        vals.push_back(vStub);
        privValidators.push_back(vStub.getPrivValidator());
    }
    std::sort(privValidators.begin(), privValidators.end(),
              [](PrivValidator one, PrivValidator other) { return one.getAddress() < other.getAddress(); });
    return ValidatorSet(vals);
}

/*//for testing
ConsensusState
common_test::randGenesisState(vector<PrivValidator> privValidators, dev::RLP genesisParams, int numValidators,
                              bool randPower, int64_t minPower) { //FIXME
    vector<PrivValidator> privVals;

    ValidatorSet validators = randValidatorSet(privVals, numValidators, randPower, minPower);
    ValidatorSet lastValidators = randValidatorSet(privVals, numValidators, randPower, minPower);
    ConsensusConfig config;
    State state(
            genesisParams[0].toString(), //chainID,
            0, //_lastBlockHeight
            BlockID(),
            boost::posix_time::from_iso_string(genesisParams[1].toString()), //lastBlockTime
            validators,
            lastValidators,
            1, //lastHeightValidatorsChanged
            ConsensusConfig::fromRLP(genesisParams[2]), //configParams;
            1, //lastHeightConsensusConfigChanged
            HexBytes(genesisParams[3].toBytes()) //appHash
    );
    ValidatorSet randomValSet = randValidatorSet(privValidators, numValidators, randPower, minPower);
    //ConsensusState cs(ConsensusConfig(), state);//, counter.NewCounterApplication(true))
    return ConsensusState(config, state);
}*/ //FIXME

void common_test::startTestRound(ConsensusState &cs, height_t height, int round) {
    cs.enterNewRound(height, round);
    cs.startRoutines();
}


// Create proposal block from cs1 but sign it with vs
Proposal common_test::decideProposal(ConsensusState &cs1, ValidatorStub &vs, height_t height, int round) {
    boost::optional<Block> block = cs1.createProposalBlock();
    if (!block.is_initialized())
        throw Panic("error creating proposal block", __FILE__, __LINE__);
    // Make proposal
    BlockID polBlockID;
    int polRound = cs1.roundState.votes.polInfo(polBlockID);
    Proposal proposal(height, round, boost::posix_time::second_clock::local_time(), polRound, polBlockID);
    try {
        vs.signProposal(cs1.state->getChainID(), proposal);
    } catch (Error &e) {
        throw Panic(e.getDescription(), __FILE__, __LINE__);
    }
    return proposal;
}

void common_test::addVotes(ConsensusState &cs, std::vector<Vote> votes) {
    for (Vote &vote : votes)
        cs.addToPeerMsgQueue(VoteMessage(AddressTm(bytes()), vote));//FIXME slicing
}

// Sign vote for type/hash/header
Vote signVote(ValidatorStub vs, VoteType type, HexBytes hash) {//, HexBytes header) {
    return vs.signVote(type, hash);//, header);
}

vector<Vote> common_test::signVotes(VoteType voteType, HexBytes hash, vector<ValidatorStub> vss) {
    vector<Vote> votes;
    for (const ValidatorStub &vs : vss) {
        votes.push_back(signVote(vs, voteType, hash));
    }
    return votes;
}

void common_test::signAddVotes(ConsensusState &cs, VoteType voteType, HexBytes hash, vector<ValidatorStub> vss) {
    vector<Vote> votes = signVotes(voteType, hash, vss);
    addVotes(cs, votes);
}

void common_test::validatePrevote(ConsensusState &cs, int round, ValidatorStub privVal, HexBytes blockHash) {
    VoteSet *prevotes = cs.roundState.votes.getPrevotes(round);
    if (!prevotes)
        throw Panic("Failed to find prevotes", __FILE__, __LINE__);
    boost::optional<Vote> vote = prevotes->getByAddress(privVal.getAddress());
    if (!vote.is_initialized())
        throw Panic("Failed to find prevote from validator", __FILE__, __LINE__);

    if (!blockHash.empty() && !vote->getBlockID().getHash().empty()) { //FIXME pasamanos
        std::string str("Expected prevote to be for nil, got %s" + vote->getBlockID().getHash().toString());
        throw Panic(str, __FILE__, __LINE__);
    }

    if (vote->getBlockID().getHash() != blockHash) {
        std::string str("Expected prevote to be for " + blockHash.toString() + ", got " +
                        vote->getBlockID().getHash().toString());
        throw Panic(str, __FILE__, __LINE__);


    }
}

void common_test::validateLastPrecommit(ConsensusState &cs, ValidatorStub &privVal, HexBytes blockHash) {
    VoteSet votes = cs.roundState.lastCommit.get();
    boost::optional<Vote> vote = votes.getByAddress(privVal.getAddress());
    if (!vote.is_initialized())
        throw Panic("Failed to find precommit from validator", __FILE__, __LINE__);
    if (vote->getBlockID().getHash() != blockHash) {
        std::string str("Expected precommit to be for ");
        str += blockHash.toString();
        str += ", got ";
        str += vote->getBlockID().getHash().toString();
        throw Panic(str, __FILE__, __LINE__);

    }
}

void common_test::validatePrecommit(ConsensusState &cs, int thisRound, int lockRound, ValidatorStub &privVal,
                                    HexBytes votedBlockHash, HexBytes lockedBlockHash) {
    VoteSet *precommits = cs.roundState.votes.getPrecommits(thisRound);
    boost::optional<Vote> vote = precommits->getByAddress(privVal.getAddress());
    if (!vote.is_initialized())
        throw Panic("Failed to find precommit from validator", __FILE__, __LINE__);
    if (votedBlockHash.empty() && vote->getBlockID().getHash().empty())
        throw Panic("Expected precommit to be for NULL", __FILE__, __LINE__);
    if (vote->getBlockID().getHash() != votedBlockHash)
        throw Panic("Expected precommit to be for proposal block", __FILE__, __LINE__);
    if (lockedBlockHash.empty() &&
        ((cs.roundState.lockedRoundNumber != lockRound || cs.roundState.lockedBlock != nullptr))) {
        std::string str("Expected to be locked on nil at round ");
        str += lockRound;
        str += ". Got locked at round ";
        str += cs.roundState.lockedRoundNumber;
        str += " with block ";
        str += cs.roundState.lockedBlock.get()->getHash().toString();
        throw Panic(str, __FILE__, __LINE__);
    }
    if (cs.roundState.lockedRoundNumber != lockRound || cs.roundState.lockedBlock.get()->getHash() != lockedBlockHash) {
        std::string str("Expected block to be locked on round ");
        str += lockRound;
        str += ", got ";
        str += cs.roundState.lockedRoundNumber;
        str += ". Got locked block ";
        str += cs.roundState.lockedBlock.get()->getHash().toString();
        str += ", expected ";
        str += lockedBlockHash.toString();

        throw Panic(str, __FILE__, __LINE__);
    }
}

void common_test::validatePrevoteAndPrecommit(ConsensusState &cs, int thisRound, int lockRound, ValidatorStub &privVal,
                                              HexBytes votedBlockHash, HexBytes lockedBlockHash) {
// verify the prevote
    validatePrevote(cs, thisRound, privVal, votedBlockHash);
// verify precommit
    std::lock_guard<std::mutex> lock(cs.mtx);
    validatePrecommit(cs, thisRound, lockRound, privVal, votedBlockHash, lockedBlockHash);
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


/*ValidatorSet common_test::randValidatorSet(vector<PrivValidator> privValidators, int nValidators, bool randPower , int64_t minPower) {
    ValidatorSet validatorSet(vector<Validator>);
    for (int i = 0; i < nValidators; i++) {
        validatorSet.add(ValidatorStub(privValidators[i], i));
        validatorSet.get(i).incrementHeight(); // since cs1 starts at 1
    }
    return validatorSet;
}*/


AddVoteResult common_test::signAddVote(PrivValidator privVal, Vote &vote, VoteSet &voteSet,
                                       boost::optional<Vote> &conflicting) {
    privVal.signVote(voteSet.chainID, vote);
    return voteSet.addVote(vote, conflicting);
}

ValidatorStub common_test::randValidator(bool randPower, int64_t minPower) {
    MockPV privVal("abcdefabcdef1234");
    int64_t votePower = minPower;
    if (randPower) {
        votePower += int64_t(std::rand());
    }
    ValidatorStub validatorStub(privVal, votePower);
    return validatorStub;
}

ValidatorStub::ValidatorStub(const MockPV _privValidator, int64_t _votePower)
        : Validator(_privValidator.getPubKey(), _votePower), privValidator(_privValidator) {
}

ValidatorStub::ValidatorStub(std::string _name, int64_t _votePower)
        : Validator(MockPV(_name).getPubKey(), _votePower), privValidator(_name) {}

const MockPV &ValidatorStub::getPrivValidator() const {
    return privValidator;
}

ValidatorStub::ValidatorStub(const PubKey &pubKey, int64_t votingPower, int index, height_t height, int round,
                             const MockPV &privValidator) : Validator(pubKey, votingPower), index(index),
                                                            height(height),
                                                            round(round), privValidator(privValidator) {}


void RoundState::setValidRoundNumber(int validRoundNumber) {
    RoundState::validRoundNumber = validRoundNumber;
}