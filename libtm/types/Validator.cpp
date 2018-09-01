//
// Created by Maru on 7/9/2018.
//

#include "Validator.h"

Validator::Validator(PubKey _pubKey, int64_t _votingPower) : pubKey(_pubKey), address(_pubKey.getAddress()) {
    votingPower = _votingPower;
    accum = 0;
}

Validator::Validator(PubKey _pubKey, int64_t _votingPower, int _index) :
        Validator(_pubKey, _votingPower) { index = _index; }

Validator::Validator(dev::Secret secret, int64_t _votingPower) : pubKey(secret), address(pubKey.getAddress()) {
    votingPower = _votingPower;
    accum = 0;
}

Validator::Validator(dev::Secret secret, int64_t _votingPower, int _index) : Validator(secret,
                                                                                       _votingPower) { index = _index; }

Validator *Validator::compareAccum(Validator &other) {
    if (this->accum > other.accum)
        return this;
    if (this->accum < other.accum)
        return &other;
    if (this->address == other.address)
        throw PanicSanity("Cannot compare identical validators", __FILE__, __LINE__);
    return this->address > other.address ? this : &other;
}

void Validator::signProposal(const std::string &chainID, Proposal &proposal) {
    if (chainID == "" || proposal.toString() == "")
        throw SignError("unable to sign proposal", __FILE__, __LINE__); //TODO
}

const AddressTm &Validator::getAddress() const {
    return address;
}

const PubKey &Validator::getPubKey() const {
    return pubKey;
}

int64_t Validator::getVotingPower() const {
    return votingPower;
}

int64_t Validator::getAccum() const {
    return accum;
}

std::string Validator::toString() const {
    return address.toString();
}

int Validator::getIndex() const {
    return index;
}

bool Validator::operator==(const Validator &other) {
    return (
            this->address == other.address &&
            this->pubKey == other.pubKey &&
            this->votingPower == other.votingPower &&
            this->accum == other.accum &&
            this->index == other.index
    );
}

void Validator::incrementAccum(int times) {
    accum += times * votingPower;
}

void Validator::decrementAccum(int64_t n) {
    accum -= n;
}

const HexBytes Validator::hash() const {
    return address.hash();
}


bool ValidatorCompareByAddress::operator()(const Validator &one, const Validator &other) {
    AddressTm t = one.getAddress();
    AddressTm o = other.getAddress();
    if (t.size() == o.size()) return 0;
    for (unsigned int i = 0; i < t.size(); ++i) {
        if (t[i] != o[i]) return (t[i] > o[i]);
    }
    return false;
}
//};