//
// Created by Maru on 7/9/2018.
//

#ifndef SRC_STATE_H
#define SRC_STATE_H

#include "../types/Validator.h"
#include "../consensus/ConsensusConfig.h"

#include <cstdint>
#include <thread>
#include <string>
#include "../types/HexBytes.h"
#include "../types/Validator.h"

using namespace std;

class State {
public:
    State();

    State(string _chainID, int _lastBlockHeight, BlockID _lastBlockID, time_t _lastBlockTime,
          ValidatorSet _validators, ValidatorSet _lastValidators, int _lastHeightValidatorsChanged,
          ConsensusConfig *_consensusParams, int _lastHeightConsensusParamsChanged, HexBytes _appHash);

    virtual ~State();

    const string &getChainID() const {
        return chainID;
    }

    int64_t getLastBlockHeight() const {
        return lastBlockHeight;
    }

    int64_t getLastBlockTotalTx() const {
        return lastBlockTotalTx;
    }

    const BlockID &getLastBlockID() const {
        return lastBlockID;
    }

    time_t getLastBlockTime() const {
        return lastBlockTime;
    }

    const ValidatorSet &getValidators() const {
        return validators;
    }

    const ValidatorSet &getLastValidators() const {
        return lastValidators;
    }

    int64_t getLastHeightValidatorsChanged() const {
        return lastHeightValidatorsChanged;
    }

    ConsensusConfig *getConsensusParams() const {
        return consensusParams;
    }

    int64_t getLastHeightConsensusParamsChanged() const {
        return lastHeightConsensusParamsChanged;
    }

    const HexBytes &getLastResultsHash() const {
        return lastResultsHash;
    }

    const HexBytes &getAppHash() const {
        return appHash;
    }

    bool isEmpty();

private:
    // Immutable
    string chainID;

    // LastBlockHeight=0 at genesis (ie. block(H=0) does not exist)
    int64_t lastBlockHeight;
    int64_t lastBlockTotalTx;
    BlockID lastBlockID;
    time_t lastBlockTime;

    // LastValidators is used to validate block.LastCommit.
    // Validators are persisted to the database separately every time they change,
    // so we can query for historical validator sets.
    // Note that if s.LastBlockHeight causes a valset change,
    // we set s.LastHeightValidatorsChanged = s.LastBlockHeight + 1
    ValidatorSet validators;
    ValidatorSet lastValidators;
    int64_t lastHeightValidatorsChanged;

    // Consensus parameters used for validating blocks.
    // Changes returned by EndBlock and updated after Commit.
    ConsensusConfig *consensusParams;
    int64_t lastHeightConsensusParamsChanged;

    // Merkle root of the results from executing prev block
    HexBytes lastResultsHash;

    // The latest AppHash we've received from calling abci.Commit()
    HexBytes appHash;
};


#endif //SRC_STATE_H
