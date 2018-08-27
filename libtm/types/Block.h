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

class TmController;

class Block {
    friend class TmController;
public:
    Block();;

    Block(height_t _height, Commit _lastCommit);

    //Block(height_t _height, Commit _lastCommit, State &state);

    bool hashesTo(HexBytes hash);

    const HexBytes &getBlockHash() const;

    const HexBytes &getAppHash() const;

    const HexBytes &getHash() const;

    const HexBytes &getBytes() const;

    height_t getHeight() const;

private:
    string chainID;
    height_t height;
    ptime time;

    int64_t numTxs;
    HexBytes hash;

    HexBytes bytes;

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
