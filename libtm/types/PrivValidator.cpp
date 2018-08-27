//
// Created by utnso on 16/07/18.
//
#include "PrivValidator.h"
#include "../../libethcore/BasicAuthority.h"

using byte = uint8_t;
using bytes = std::vector<byte>;
using height_t = uint64_t;

PrivValidator::PrivValidator(PrivKey _privKey) : addresstm(_privKey.getAddress()), privKey(_privKey) {
};

std::string PrivValidator::toString() const {
    return this->getAddress().toString();
}

Address PrivValidator::getAddress() const {
    return addresstm;
}

void PrivValidator::signProposal(string, Proposal &) {
    //TODO
}

void PrivValidator::signVote(string chainId, Vote &vote) {
    //TODO
    Signature sig = this->privKey.sign(chainId);
    HexBytes hash = vote.getBlockID().getHash();

    bytes _bytes;
    for (byte b : sig) {
        _bytes.push_back(b);
    }
    for (byte h : hash) {
        _bytes.push_back(h);
    }
    vote.setSignature(HexBytes(_bytes));
}


const PubKey &PrivValidator::getPubKey() const {
    return privKey.getPubKey();
}