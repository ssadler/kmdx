//
// Created by utnso on 11/07/18.
//

#include "Proposal.h"

// If there is no POLRound, polRound should be -1.
Proposal::Proposal(height_t _height, int _round, boost::posix_time::ptime _timestamp, int _polRound,
                   BlockID _polBlockID) :
        height(_height), roundNumber(_round), timestamp(_timestamp), polRound(_polRound), polBlockID(_polBlockID) {}

Proposal::Proposal(height_t _height, int _round, boost::posix_time::ptime _timestamp, int _polRound,
                   BlockID _polBlockID, SignatureTm _signature) : Proposal(_height, _round, _timestamp, _polRound, _polBlockID) {
    signature = _signature;

}
height_t Proposal::getHeight() const {
    return height;
}

int Proposal::getPolRound() const {
    return polRound;
}

int Proposal::getRoundNumber() const {
    return roundNumber;
}

string Proposal::toString() const {
    ostringstream out;
    out << height;
    out << roundNumber;
    out << boost::posix_time::to_iso_string(timestamp);
    out << block->getBytes().toString();
    out << polRound;
    out << polBlockID.toString();
    out << signature.toString();
    return out.str();
}

Proposal Proposal::fromRLP(dev::RLP r) {
//    BlockID bid = BlockID::fromRLP(r[4]);
    return Proposal(
            (height_t) r[0].toPositiveInt64() // height
            , r[1].toInt() // roundNumber
            , boost::posix_time::from_iso_string(r[2].toString()) // timestamp
//            , r[3].toInt() // block
            , r[3].toInt() // polRound
//            , BlockID()
            , BlockID::fromRLP(r[4])// bid // polBlockID
            , r[5].isData() ? SignatureTm(r[5].toBytes()) : SignatureTm()
    );
}

dev::RLPStream Proposal::toRLP() {
    dev::RLPStream rlp;
    bool withSignature = !signature.empty();
    rlp.appendList(withSignature ? 6 : 5);
    rlp << height;
    rlp << roundNumber;
    rlp << boost::posix_time::to_iso_string(timestamp);
//    rlp << block.toRLP();
    rlp << polRound;
    rlp << dev::RLP(polBlockID.toRLP().out());
    if (withSignature)
        rlp << signature;
    return rlp;
}
