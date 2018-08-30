//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_PROPOSAL_H
#define TM_LIGHT_PROPOSAL_H


#include <ctime>
#include <cstdint>
#include "HexBytes.h"
#include "Block.h"
/*#include "RoundState.h"*/
#include "boost/date_time/posix_time/posix_time_types.hpp"



// Proposal defines a block proposal for the consensus.
// It refers to the block only by its PartSetHeader.
// It must be signed by the correct proposer for the given Height/Round
// to be considered valid. It may depend on votes from a previous roundNumber,
// a so-called Proof-of-Lock (POL) roundNumber, as noted in the POLRound and POLBlockID.
class Proposal {
    height_t height;
    int roundNumber;
    boost::posix_time::ptime timestamp;
    shared_ptr<Block> block;//should be type blockPartsHeader;
    int polRound; // -1 if null.
    BlockID polBlockID; // zero if null.
    SignatureTm signature;

    friend class MockPV;

public:
    Proposal(height_t _height, int _round, int _polRound, BlockID _polBlockID);

    HexBytes signBytes(const string blockChainId) const;

    height_t getHeight() const;

    int getRoundNumber() const;

    int getPolRound() const;

    const SignatureTm &getSignature() const;

    string toString() const;

};


#endif //TM_LIGHT_PROPOSAL_H
