//
// Created by utnso on 11/07/18.
//

#include "Proposal.h"

// If there is no POLRound, polRound should be -1.
Proposal::Proposal(int64_t _height, int _round, int _polRound, BlockID _polBlockID) : polBlockID(
        _polBlockID.isEmpty() ? BlockID() : _polBlockID) {
    height = _height;
    roundNumber = _round;
    timestamp = boost::posix_time::second_clock::local_time();
    polRound = _polRound;
    polBlockID = _polBlockID;

}

int64_t Proposal::getHeight() const {
    return height;
}


int Proposal::getPolRound() const {
    return polRound;
}

const Signature &Proposal::getSignature() const {
    return signature;
}

int Proposal::getRoundNumber() const {
    return roundNumber;
}

HexBytes Proposal::signBytes(const string blockChainId) const {
    return HexBytes(blockChainId); //TODO
}

string Proposal::toString() const {
    return "Proposal"; //FIXME
}