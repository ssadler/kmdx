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
};

Vote Vote::fromRLP(dev::RLP &r) {
    return Vote(
            PeerID(r[1].toBytes()),
            r[2].toInt(),
            r[3].toInt<int64_t>(),
            r[4].toInt(),
            boost::posix_time::from_iso_string(r[5].toString()),
            Vote::allVoteTypes[r[6].toInt()], BlockID(r[7].toBytes()),
            Signature(r[8].toBytes())
    );
}

VoteType Vote::allVoteTypes[VoteTypeSize] = {
        VoteTypePrevote, VoteTypePrecommit, VoteTypeFirstCommit,
};

dev::RLP Vote::toRLP() {
    dev::RLPStream rlp(8);
    rlp.append(validatorAddress);
    rlp.append(validatorIndex);
    rlp.append(height); //FIXME int64 might not fit
    rlp.append(roundNumber);
    rlp.append(boost::posix_time::to_iso_string(timestamp));
    rlp.append(type);
    rlp.append(blockID.getBites());
    rlp.append(signature);
    return dev::RLP(rlp.out());
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

Vote::Vote(int _validatorIndex, const HexBytes privValAddress, int64_t _height, int _roundNumber,
           boost::posix_time::ptime _timestamp, VoteType _type, BlockID _blockId)
        : validatorAddress(privValAddress), blockID(_blockId), signature() {
    validatorIndex = _validatorIndex;
    height = _height;
    roundNumber = _roundNumber;
    timestamp = _timestamp;
    type = _type;
}

Vote::Vote(VoteType _type)
        : validatorAddress(), blockID(), signature() {
//    validatorIndex = 0;
//    height = 0;
//    roundNumber = 0;
    timestamp = boost::posix_time::second_clock::local_time();
    type = _type;
}

void Vote::verify(const std::string &chainID, const Pubkey &pubKey) const {
    if (pubKey.getAddress() != this->validatorAddress) {
        throw ErrInvalidVote("invalid validator Address");
    }
    if (!pubKey.verifyBytes(this->signBytes(chainID), this->signature)) {
        throw ErrorInvalidSignature("vote");
    }
}

std::string Vote::toString() const {
    //TODO
    return Vote::voteTypeToString(type) + " " + validatorAddress.toString();
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

HexBytes Vote::signBytes(std::string chainID) const {
    cout << chainID;
    return HexBytes(); //TODO
}

