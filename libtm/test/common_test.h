//
// Created by thpn on 04/08/18.
//

#ifndef AYETH_COMMON_TEST_H
#define AYETH_COMMON_TEST_H

#include <libdevcore/RLP.h>
#include <utility>

#include "../consensus/ConsensusState.h"
#include "../types/VoteSet.h"
#include "../types/Validator.h"
#include "../state/State.h"
#include "MockPV.h"

//class ConsensusState;

using byte = uint8_t;
using bytes = std::vector<byte>;

//a validator we control
class ValidatorStub : public Validator {
private :
    int index; // Validator index. NOTE: we don't assume validator set changes.
    int64_t height;
    int round;
    MockPV privValidator;

    friend class common_test;

public:
    ValidatorStub(const PubKey &pubKey, int64_t votingPower, int index, int64_t height, int round,
                  const MockPV &privValidator);

    ValidatorStub(const MockPV _privValidator, int64_t _votePower);

    const MockPV &getPrivValidator() const;

    Vote signVote(VoteType voteType, HexBytes hash);//, bytes header);
};

class common_test {
public:
    static ValidatorSet
    randValidatorSet(std::vector<PrivValidator> &privValidators, int numValidators, int64_t votingPower);

    static ValidatorSet
    randValidatorSet(std::vector<PrivValidator> &privValidators, int numValidators, bool randPower,
                     int64_t votingPower);

    static ValidatorStub randValidator(bool randPower, int64_t minPower);

    static ConsensusState randGenesisState(vector<PrivValidator> privValidators, dev::RLP genesisParams,
                                           int numValidators, bool randPower, int64_t minPower);

    void startTestRound(ConsensusState &cs, int64_t height, int round);


    Proposal decideProposal(ConsensusState &cs1, ValidatorStub &vs, int64_t height, int round);

    void signAddVotes(ConsensusState &cs, VoteType voteType, HexBytes hash, vector<ValidatorStub> vss);

    void validatePrevote(ConsensusState &cs, int round, ValidatorStub privVal, HexBytes blockHash);

    void validateLastPrecommit(ConsensusState &cs, ValidatorStub &privVal, HexBytes blockHash);

    void validatePrecommit(ConsensusState &cs, int thisRound, int lockRound, ValidatorStub &privVal,
                           HexBytes votedBlockHash, HexBytes lockedBlockHash);

    void validatePrevoteAndPrecommit(ConsensusState &cs, int thisRound, int lockRound, ValidatorStub &privVal,
                                     HexBytes votedBlockHash, HexBytes lockedBlockHash);

    void subscribeToVoter(ConsensusState &cs, Address address);

    bool static signAddVote(PrivValidator privVal, Vote &vote, VoteSet &voteSet);

private :
    vector<Vote> signVotes(VoteType voteType, HexBytes hash, vector<ValidatorStub> vss);

    void addVotes(ConsensusState &cs, std::vector<Vote> votes);

};

/*
void VoteSet::setMaj23(BlockID &_maj23) {
    VoteSet::maj23 = _maj23;
}
*/


#endif //AYETH_COMMON_TEST_H
