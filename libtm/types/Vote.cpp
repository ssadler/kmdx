//
// Created by utnso on 12/07/18.
//

#include <boost/date_time/posix_time/posix_time.hpp>
#include "HexBytes.h"
#include "Vote.h"


Vote Vote::fromRLP(dev::RLP & r) {
    return Vote(
            PeerID(r[1].toVector<uint8_t>()),
            r[2].toInt(),
            r[3].toInt<int64_t>(),
            r[4].toInt(),
            boost::posix_time::from_iso_string(r[5].toString()),
            Vote::allVoteTypes[r[6].toInt()], BlockID(r[7].toInt<int64_t>()), Signature()
//Signature::fromRLP(r[8].RLPs())
    );
}

dev::u256 Vote::toRLP() {
    dev::RLPStream rlp(8);
    rlp.append(validatorAddress.getBites());
    rlp.append(validatorIndex);
    rlp.append(height);
    rlp.append(roundNumber);
    rlp.append(boost::posix_time::to_iso_string(timestamp));
    rlp.append(type);
    rlp.append(blockID.getBites().getBites());
    rlp.append(signature.toRLP());
    return rlp;
    //return dev::u256(rlp.out());
}


Vote::Vote(const PeerID peerID, int _validatorIndex, int64_t _height, int _roundNumber,
           const boost::posix_time::ptime &_timestamp, VoteType _type, const BlockID _blockID,
           const Signature _signature)
        : validatorAddress(peerID), blockID(_blockID), signature(_signature) {
    validatorIndex = _validatorIndex;
    height = _height;
    roundNumber = _roundNumber;
    timestamp = _timestamp;
    type = _type;
}


const HexBytes &Vote::getValidatorAddress() const {
    return validatorAddress;
}

int Vote::getValidatorIndex() const {
    return validatorIndex;
}

int64_t Vote::getHeight() const {
    return height;
}

int Vote::getRoundNumber() const {
    return roundNumber;
}

boost::posix_time::ptime Vote::getTimestamp() const {
    return timestamp;
}

VoteType Vote::getType() const {
    return type;
}

const BlockID &Vote::getBlockID() const {
    return blockID;
}

const Signature &Vote::getSignature() const {
    return signature;
}



int main (){
    PeerID pid(std::vector<uint8_t >(1234));
BlockID bid (std::vector<uint8_t >(1234));

    Vote v(pid, 1, int64_t(2), 3,
        boost::posix_time::second_clock::local_time(),
        Vote::allVoteTypes[1], bid);
    dev::u256 rlp = v.toRLP();
    Vote o = Vote::fromRLP(RLP(rlp.out()));
    cout<<o.getHeight();
}