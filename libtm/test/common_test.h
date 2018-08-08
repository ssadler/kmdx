//
// Created by thpn on 04/08/18.
//

#ifndef AYETH_COMMON_TEST_H
#define AYETH_COMMON_TEST_H

#include <libdevcore/RLP.h>
#include <utility>
#include "../types/Validator.h"
#include "../consensus/ConsensusState.h"
#include "../state/State.h"

using byte = uint8_t;
using bytes = std::vector<byte>;

std::string chain_ID;

//a validator we control
class ValidatorStub : public Validator {
private :
    int index; // Validator index. NOTE: we don't assume validator set changes.
    int64_t height;
    int round;
    PrivValidator privValidator;

    friend class common_test;

public:
    ValidatorStub(const Pubkey &pubKey, int64_t votingPower, int index, int64_t height, int round,
                  const PrivValidator &privValidator) : Validator(pubKey, votingPower), index(index), height(height),
                                                        round(round), privValidator(privValidator) {}

private:
    Vote signVote(VoteType voteType, bytes hash);
};

class common_test {
public:
    static ValidatorSet
    randValidatorSet(vector<PrivValidator> privValidators, int numValidators, bool randPower, int64_t minPower);

    static PrivValidator randValidator(bool randPower, int64_t minPower);

    static ConsensusState randGenesisState(vector<PrivValidator> privValidators, dev::RLP genesisParams,
                                           int numValidators, bool randPower, int64_t minPower);

    void startTestRound(ConsensusState &cs, int64_t height, int round);


    Proposal decideProposal(ConsensusState &cs1, ValidatorStub &vs, int64_t height, int round);

    void signAddVotes(ConsensusState &cs, VoteType voteType, bytes hash, vector<ValidatorStub> vss);

    void validatePrevote(ConsensusState &cs, int round, ValidatorStub privVal, HexBytes blockHash);

    void validateLastPrecommit(ConsensusState &cs, ValidatorStub &privVal, HexBytes blockHash);

    void validatePrecommit(ConsensusState &cs, int thisRound, int lockRound, ValidatorStub &privVal,
                           HexBytes votedBlockHash, HexBytes lockedBlockHash);

    void validatePrevoteAndPrecommit(ConsensusState &cs, int thisRound, int lockRound, ValidatorStub &privVal,
                                     HexBytes votedBlockHash, HexBytes lockedBlockHash);

    void subscribeToVoter(ConsensusState &cs, Address address);

private :
    vector<Vote> signVotes(VoteType voteType, bytes hash, vector<ValidatorStub> vss);

    void addVotes(ConsensusState &cs, std::vector<Vote> votes);

};


#endif //AYETH_COMMON_TEST_H
