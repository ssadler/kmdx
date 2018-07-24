//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_COMMIT_H
#define TM_LIGHT_COMMIT_H

#include <memory>
#include "HexBytes.h"
#include "Vote.h"
#include "Error.h"

/* Commit contains the evidence that a block was committed by a set of validators.
 NOTE: Commit is empty for height 1, but never nullptr.*/
class Commit {

    // NOTE: The Precommits are in order of address to preserve the bonded ValidatorSet order.
    // Any peer with a block can gossip precommits by index with a peer without recalculating the
    // active ValidatorSet.
    BlockID blockID;
    std::vector<std::shared_ptr<Vote>> precommits;

    // Volatile
    shared_ptr<Vote> firstPrecommit;
    HexBytes hash;
    std::vector<bool> bitArray;

public:
    Commit(); //FIXME maybe delete
    shared_ptr<Vote> getFirstPrecommit();

    int round();

    int64_t height();

    VoteType type();

    ulong size();

    bool isCommit();

    void validateBasic(); // throw(ErrInvalidVoteSet);

    vector<shared_ptr<Vote>> getPrecommits() const;

    HexBytes getHash();
};


#endif //TM_LIGHT_COMMIT_H
