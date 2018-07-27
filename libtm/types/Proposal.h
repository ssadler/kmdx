//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_PROPOSAL_H
#define TM_LIGHT_PROPOSAL_H


#include <ctime>
#include <cstdint>
#include "HexBytes.h"
#include "Signature.h"
#include "Block.h"
#include "RoundState.h"
#include "boost/date_time/posix_time/posix_time_types.hpp"



// Proposal defines a block proposal for the consensus.
// It refers to the block only by its PartSetHeader.
// It must be signed by the correct proposer for the given Height/Round
// to be considered valid. It may depend on votes from a previous roundNumber,
// a so-called Proof-of-Lock (POL) roundNumber, as noted in the POLRound and POLBlockID.
class Proposal {
    int64_t height;
    int roundNumber;
    boost::posix_time::ptime timestamp;
    shared_ptr<Block> block;//should be type blockPartsHeader;
    int polRound; // -1 if null.
    BlockID polBlockID; // zero if null.
    Signature signature;


public:

// If there is no POLRound, polRound should be -1.
    Proposal(int64_t _height, int _round, int _polRound, BlockID _polBlockID) : polBlockID(
            _polBlockID.isEmpty() ? BlockID(-1) : _polBlockID) {
        height = _height;
        roundNumber = _round;
        timestamp = boost::posix_time::second_clock::local_time();
        polRound = _polRound;
        polBlockID = _polBlockID;

    }

    const HexBytes &signBytes(const string blockChainId) const;

    int64_t getHeight() const;

    const RoundState &getRoundState() const;

    int getPolRound() const;

    const Signature &getSignature() const;

    string toString() const;

};


#endif //TM_LIGHT_PROPOSAL_H
