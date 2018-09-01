//
// Created by thpn on 13/08/18.
//


#include "MockPV.h"

//----------------------------------------
// MockPV

// MockPV implements PrivValidator without any safety or persistence.
// Only use it for testing.

MockPV::MockPV(PrivKey _privkey) : PrivValidator(_privkey) {
}

MockPV::MockPV(std::string _name) : PrivValidator(dev::Secret(_name)), name(_name) {}

// Implements PrivValidator.
AddressTm MockPV::getAddress() const {
    return privKey.getPubKey().getAddress();
}

// Implements PrivValidator.
PubKey MockPV::getPubKey() const {
    return privKey.getPubKey();
}

// Implements PrivValidator.
void MockPV::signVote(std::string chainID, Vote &vote) const {
    HexBytes signBytes = vote.signBytes(chainID);
    HexBytes sig = privKey.sign(signBytes);
    vote.signature = sig;
}

// Implements PrivValidator.
void MockPV::signProposal(std::string chainID, Proposal &proposal) const {
    HexBytes signBytes = proposal.signBytes(chainID);
    SignatureTm sig = privKey.sign(signBytes);

    proposal.signature = sig;
}

// signHeartbeat signs the heartbeat without any checking.
void MockPV::signHeartbeat(std::string chainID, Heartbeat &heartbeat) const {
    SignatureTm sig = privKey.sign(heartbeat.signBytes(chainID));
    heartbeat.signature = sig;
}

// String returns a string representation of the MockPV.
std::string MockPV::toString() const {
    std::ostringstream out;
    out << "MockPV{%s}", getAddress().toString();
    return out.str();
}
