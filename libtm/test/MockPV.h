//
// Created by thpn on 13/08/18.
//

#ifndef AYETH_MOCKPV_H
#define AYETH_MOCKPV_H

#include "../types/Vote.h"
#include "../types/Proposal.h"
#include "../types/HexBytes.h"
#include "../types/PrivValidator.h"
#include "../types/Heartbeat.h"


class MockPV : public PrivValidator {
public:
    std::string name;

    MockPV(PrivKey _privkey);

    MockPV(std::string _name);

    void signVote(std::string chainID, Vote &vote) const;

    void signProposal(std::string chainID, Proposal &proposal) const;

    void signHeartbeat(std::string chainID, Heartbeat &heartbeat) const;

    AddressTm getAddress() const;

    PubKey getPubKey() const;

    std::string toString() const;

};


#endif //AYETH_MOCKPV_H
