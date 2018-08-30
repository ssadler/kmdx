//
// Created by utnso on 16/07/18.
//
#include "PrivValidator.h"
//#include "../../libethcore/BasicAuthority.h"

using byte = uint8_t;
using bytes = std::vector<byte>;
using height_t = uint64_t;

PrivValidator::PrivValidator(PrivKey _privKey) : addresstm(_privKey.getAddress()), privKey(_privKey) {
};

std::string PrivValidator::toString() const {
    return this->getAddress().toString();
}

AddressTm PrivValidator::getAddress() const {
    return addresstm;
}

void PrivValidator::signProposal(string, Proposal &) {
    //TODO
}

void PrivValidator::signVote(string chainId, Vote &vote) {
    //TODO
    cout << chainId;
    dev::Secret s(privKey.getKey());
    dev::Signature sig = dev::sign(s, dev::sha3(vote.toRLP().out()));
    vote.setSignature(SignatureTm(sig.asBytes()));
}


const PubKey &PrivValidator::getPubKey() const {
    return privKey.getPubKey();
}

const PrivKey &PrivValidator::getPrivKey() const {
    return privKey;
}
