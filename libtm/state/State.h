//
// Created by thpn on 02/08/18.
//

#ifndef AYETH_STATE_H
#define AYETH_STATE_H

#include "../types/ValidatorSet.h"
#include "../consensus/ConsensusConfig.h"

#include <cstdint>
#include <thread>
#include <string>
#include "../types/HexBytes.h"
#include "../types/ValidatorSet.h"


class State {
public:
    State();

    State(std::string _chainID, height_t _lastBlockHeight, BlockID _lastBlockID,
          boost::posix_time::ptime _lastBlockTime,
          ValidatorSet _validators, ValidatorSet _lastValidators, height_t _lastHeightValidatorsChanged,
          ConsensusConfig _consensusParams, height_t _lastHeightConsensusParamsChanged, HexBytes _appHash);

//    virtual ~State();

    const std::string &getChainID() const;

    height_t getLastBlockHeight() const;

    int64_t getLastBlockTotalTx() const;

    const BlockID &getLastBlockID() const;

    boost::posix_time::ptime getLastBlockTime() const;

    ValidatorSet &getValidators();

    const ValidatorSet &getLastValidators() const;

    height_t getLastHeightValidatorsChanged() const;

    ConsensusConfig getConsensusParams() const;

    height_t getLastHeightConsensusParamsChanged() const;

    const HexBytes &getLastResultsHash() const;

    const HexBytes &getAppHash() const;

private:
    // Immutable
    std::string chainID;

    // LastBlockHeight=0 at genesis (ie. block(H=0) does not exist)
    height_t lastBlockHeight;
    int64_t lastBlockTotalTx;
    BlockID lastBlockID;
    boost::posix_time::ptime lastBlockTime;

    // LastValidators is used to validate block.LastCommit.
    // Validators are persisted to the database separately every time they change,
    // so we can query for historical validator sets.
    // Note that if s.LastBlockHeight causes a valset change,
    // we set s.LastHeightValidatorsChanged = s.LastBlockHeight + 1
    ValidatorSet validators;
    ValidatorSet lastValidators;
    height_t lastHeightValidatorsChanged;

    // Consensus parameters used for validating blocks.
    // Changes returned by EndBlock and updated after Commit.
    ConsensusConfig consensusParams;
    height_t lastHeightConsensusParamsChanged;

    // Merkle root of the results from executing prev block
    HexBytes lastResultsHash;

    // The latest AppHash we've received from calling abci.Commit()
    HexBytes appHash;

    //for testing
    static State randGenesisState(int, bool, height_t); //TODO
};


#endif //AYETH_STATE_H
