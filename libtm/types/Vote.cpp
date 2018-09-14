//
// Created by utnso on 12/07/18.
//

#include "Vote.h"


std::string Vote::voteTypeToString(VoteType type) {
    switch (type) {
        case VoteTypePrevote:
            return "VoteTypePrevote";
        case VoteTypePrecommit:
            return "VoteTypePrecommit";
        case VoteTypeFirstCommit:
            return "VoteTypeFirstCommit";
        default:
            return "invalid Vote type";
    }
}

Vote Vote::fromRLP(dev::RLP &r) {
    return Vote(
            AddressTm(r[0].toBytes()),
            r[1].toInt(),
            (height_t) r[2].toPositiveInt64(),
            r[3].toInt(),
            boost::posix_time::from_iso_string(r[4].toString()),
            Vote::allVoteTypes[r[5].toInt()],
            BlockID(r[6].toBytes()),
            r[7].isData() ? SignatureTm(r[7].toBytes()) : SignatureTm()
    );
}

Vote::Vote(const Vote &v) : Vote(v.validatorAddress, v.validatorIndex, v.height, v.roundNumber, v.timestamp, v.type,
                                 v.blockID, v.signature) {

}

VoteType Vote::allVoteTypes[VoteTypeSize] = {
        VoteTypePrevote, VoteTypePrecommit, VoteTypeFirstCommit,
};

dev::RLPStream Vote::toRLP() const {
    dev::RLPStream rlp;
    bool withSignature = !signature.empty();
    rlp.appendList(withSignature ? 8 : 7);
    rlp << validatorAddress;
    rlp << validatorIndex;
    rlp << height;
    rlp << roundNumber;
    rlp << boost::posix_time::to_iso_string(timestamp);
    rlp << (int) type;
    rlp << blockID.getBytes();
    if (withSignature)
        rlp << signature;
    return rlp;
}


Vote::Vote(const AddressTm addresstm, int _validatorIndex, height_t _height, int _roundNumber,
           const boost::posix_time::ptime &_timestamp, VoteType _type, const BlockID _blockID)
        : validatorAddress(addresstm), blockID(_blockID) {
    validatorIndex = _validatorIndex;
    height = _height;
    roundNumber = _roundNumber;
    timestamp = _timestamp;
    type = _type;
}

Vote::Vote(const AddressTm addresstm, int _validatorIndex, height_t _height, int _roundNumber,
           const boost::posix_time::ptime &_timestamp, VoteType _type, const BlockID _blockID,
           const SignatureTm _signature) : Vote(addresstm, _validatorIndex, _height, _roundNumber, _timestamp, _type,
                                                _blockID) {
    signature = _signature;
}


Vote::Vote(VoteType _type) {//: validatorAddress(), blockID(), signature() {
//    validatorIndex = 0;
//    height = 0;
//    roundNumber = 0;
    timestamp = boost::posix_time::second_clock::local_time();
    type = _type;
}

void Vote::verify(const std::string &chainID, const PubKey &pubKey) const {
    if (pubKey.getAddress() != this->validatorAddress) {
        throw ErrInvalidVote("invalid validator Address", __FILE__, __LINE__);
    }
    if (!pubKey.verifyBytes(this->signBytes(chainID), this->signature)) {
        throw ErrorInvalidSignature("vote", __FILE__, __LINE__);
    }
}

std::string Vote::toString() const {
    ostringstream out;
    out << validatorAddress.toString();
    out << validatorIndex;
    out << height;
    out << roundNumber;
    out << boost::posix_time::to_iso_string(timestamp);
    out << (int) type;
    out << blockID.getBytes().toString();
    if (!signature.empty())
        out << signature.toString();
    return out.str();
}

const AddressTm &Vote::getValidatorAddress() const {
    return validatorAddress;
}

int Vote::getValidatorIndex() const {
    return validatorIndex;
}

height_t Vote::getHeight() const {
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

//const SignatureTm &Vote::getSignature() const {
//    return signature;
//}

/*HexBytes Vote::signBytes(std::string chainID) const {
    HexBytes out(dev::sha3(chainID + toString()).asBytes());
    return out;
//    std::string got = out.str();
//    return HexBytes(out.str()); //TODO
}*/
/*
void Vote::setSignature(const SignatureTm &signature) {
    Vote::signature = signature;
}*/

bool Vote::isVoteTypeValid(VoteType type) {
    switch (type) {
        case VoteTypePrevote:
        case VoteTypePrecommit:
            return true;
        default:
            return false;
    }
}

