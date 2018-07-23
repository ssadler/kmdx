//
// Created by Maru on 7/9/2018.
//

#include <cstdint>
#include <ctime>
#include "Block.h"

// Create a block from the latest state
Block::Block(int64_t _height, Commit _lastCommit) {
    height = _height;
    lastCommit = _lastCommit;
    time = time_t(NULL);
}


// builds a block with the given txs and commit from the current state.
Block::Block(int64_t _height, vector<Tx> txs, Commit _lastCommit, State &_state) : Block(_height, _lastCommit) {
    chainID = _state.getChainID();
    totalTxs = _state.getLastBlockTotalTx() + numTxs;
    lastBlockID = _state.getLastBlockID();
    validatorsHash = _state.getValidators().hash();
    appHash = _state.getAppHash();
    consensusHash = (*_state.getConsensusParams()).hash();
    lastResultsHash = _state.getLastResultsHash();
    //TODO block.makePartSet(state.consensusParams.blockGossip.blockPartSizeBytes);
}

