//
// Created by utnso on 11/07/18.
//

#include "Proposal.h"

// If there is no POLRound, polRound should be -1.
Proposal::Proposal(height_t _height, int _round, boost::posix_time::ptime _timestamp, int _polRound,
                   BlockID _polBlockID) :
        height(_height), roundNumber(_round), timestamp(_timestamp), polRound(_polRound), polBlockID(_polBlockID) {}

height_t Proposal::getHeight() const {
    return height;
}

int Proposal::getPolRound() const {
    return polRound;
}

const SignatureTm &Proposal::getSignature() const {
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

Proposal Proposal::fromRLP(dev::RLP r) {
    BlockID bid = BlockID::fromRLP(r[4]);
    return Proposal(
            (height_t) r[0].toPositiveInt64() // height
            , r[1].toInt() // roundNumber
            , boost::posix_time::from_iso_string(r[2].toString()) // timestamp
//            , r[3].toInt() // block
            , r[3].toInt() // polRound
//            , BlockID()
            , bid // polBlockID
//            , SignatureTm(r[6].toBytes()) // signature
    );
}

dev::RLPStream Proposal::toRLP() {
    dev::RLPStream rlp;
    rlp.appendList(5);
    rlp << height;
    rlp << roundNumber;
    rlp << boost::posix_time::to_iso_string(timestamp);
//    rlp << block.toRLP();
    rlp << polRound;
    rlp << dev::RLP(polBlockID.toRLP().out());
//    rlp << signature;
    return rlp;
}
