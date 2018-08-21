//
// Created by thpn on 02/08/18.
//

#include "State.h"

const std::string &State::getChainID() const {
    return chainID;
}

int64_t State::getLastBlockHeight() const {
    return lastBlockHeight;
}

int64_t State::getLastBlockTotalTx() const {
    return lastBlockTotalTx;
}

const BlockID &State::getLastBlockID() const {
    return lastBlockID;
}

boost::posix_time::ptime State::getLastBlockTime() const {
    return lastBlockTime;
}

ValidatorSet &State::getValidators() {
    return validators;
}

const ValidatorSet &State::getLastValidators() const {
    return lastValidators;
}

int64_t State::getLastHeightValidatorsChanged() const {
    return lastHeightValidatorsChanged;
}

ConsensusConfig State::getConsensusParams() const {
    return consensusParams;
}

int64_t State::getLastHeightConsensusParamsChanged() const {
    return lastHeightConsensusParamsChanged;
}

const HexBytes &State::getLastResultsHash() const {
    return lastResultsHash;
}

const HexBytes &State::getAppHash() const {
    return appHash;
}
    
/* State is a short description of the latest committed block of the Tendermint consensus.
* It keeps all information necessary to validate new blocks,
* including the last validator set and the consensus params.
* Instead, use state.Copy() or state.NextState(...).*/



/*State::~State() {
    //TODO state destructor
}*/



//State::State(const State &obj){
//TODO copy constructor
//}

//------------------------------------------------------------------------
// Genesis

// MakeGenesisStateFromFile reads and unmarshals state from the given
// file.
//
// Used during replay and in tests.
/** needs an initialized state */

/*std::string _chainID, int _lastBlockHeight, BlockID _lastBlockID, time_t _lastBlockTime,
        ValidatorSet _validators, ValidatorSet _lastValidators, int _lastHeightValidatorsChanged,
        ConsensusConfig *_consensusParams, int _lastHeightConsensusParamsChanged, HexBytes _appHash*/

