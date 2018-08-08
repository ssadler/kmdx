#ifndef SRC_BLOCK_H
#define SRC_BLOCK_H

#include <mutex>
#include <string>
#include <vector>
#include "Commit.h"
#include "HexBytes.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace boost::posix_time;
using namespace std;

#include <cstdint>
#include <vector>
#include <libdevcore/RLP.h>

using byte = uint8_t;
using bytes = std::vector<byte>;


class Block {
public:
    Block(int64_t height, Commit lastCommit);
    //Block(int64_t _height, Commit _lastCommit, State &state);

    bool hashesTo(HexBytes hash);

    const string &getChainID() const;

    int64_t getHeight() const;

    time_t getTime() const;

    int64_t getNumTxs() const;

    const Commit &getLastCommit() const;

    const BlockID &getLastBlockID() const;

    uint64_t getTotalTxs() const;

    const HexBytes &getBlockHash() const;

    const HexBytes &getLastCommitHash() const;

    const HexBytes &getDataHash() const;

    const HexBytes &getValidatorsHash() const;

    const HexBytes &getConsensusHash() const;

    const HexBytes &getAppHash() const;

    const HexBytes &getLastResultsHash() const;

    HexBytes getHash();

    bytes getBytes();


private:

    string chainID;
    int64_t height;
    ptime time;
    int64_t numTxs;

    Commit lastCommit;

    // prev block info
    BlockID lastBlockID;
    uint64_t totalTxs;

    HexBytes blockHash;
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
