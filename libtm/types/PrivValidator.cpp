//
// Created by utnso on 16/07/18.
//
#include "PrivValidator.h"

PrivValidator::PrivValidator(PrivKey _privKey) : privKey(_privKey) {
};

std::string PrivValidator::toString() const {
    return this->getAddress().toString();
}

Address PrivValidator::getAddress() const {
    return address;
}

void PrivValidator::signProposal(string, Proposal &) {
//TODO
}

void PrivValidator::signVote(string, Vote &) {
//TODO
}


const PubKey &PrivValidator::getPubKey() const {
    return privKey.getPubKey();
}