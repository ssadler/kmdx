//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_PROPOSAL_H
#define TM_LIGHT_PROPOSAL_H


#include <ctime>
#include "HexBytes.h"
#include "Signature.h"
#include "Block.h"
#include "RoundState.h"

// Proposal defines a block proposal for the consensus.
// It refers to the block only by its PartSetHeader.
// It must be signed by the correct proposer for the given Height/Round
// to be considered valid. It may depend on votes from a previous roundNumber,
// a so-called Proof-of-Lock (POL) roundNumber, as noted in the POLRound and POLBlockID.
class Proposal {
    int64_t height;
    RoundState roundState;
    time_t Timestamp;
    Block block;//should be type blockPartsHeader;
    int polRound; // -1 if null.
    HexBytes polBlockID; // zero if null.
    Signature signature;


public:

    const HexBytes &signBytes(const string blockChainId) const {
        //TODO
    }

    int64_t getHeight() const;

    const RoundState &getRoundState() const;

    time_t getTimestamp() const;

    const Block &getBlock() const;

    int getPolRound() const;

    const HexBytes &getPolBlockID() const;

    const Signature &getSignature() const;

    const string toString() const;
};


#endif //TM_LIGHT_PROPOSAL_H
