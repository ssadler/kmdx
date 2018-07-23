#ifndef SRC_BLOCK_H
#define SRC_BLOCK_H

#include <mutex>
#include <string>
#include <vector>
#include "Commit.h"
#include "Tx.h"
#include "HexBytes.h"

using namespace std;

class State;

class Block {
public:
    Block(int64_t height, Commit lastCommit);

    Block(int64_t _height, vector<Tx> txs, Commit _lastCommit, State &state);

    bool hashesTo(HexBytes hash);

private:

    string chainID;
    int64_t height;
    time_t time;
    int64_t numTxs;

    Commit lastCommit;

    // prev block info
    BlockID lastBlockID;
    uint64_t totalTxs;

    HexBytes BlockHash;
    // hashes of block data
    HexBytes lastCommitHash; // commit from validators from the last block
    HexBytes dataHash; // transactions

    // hashes from the app output from the prev block
    HexBytes validatorsHash; // validators for the current block
    HexBytes consensusHash; // consensus params for current block
    HexBytes appHash; // state after txs from the previous block
    HexBytes lastResultsHash; // root hash of all results from the txs from the previous block
};


#endif //SRC_BLOCK_H
