//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_PRIVVALIDATOR_H
#define TM_LIGHT_PRIVVALIDATOR_H


#include "HexBytes.h"
#include "Proposal.h"

class PrivValidator {

protected:
    Address address;
    PrivKey privKey;
public:
    PrivValidator(const PrivKey _privKey);

    PrivValidator();

    Address getAddress() const;

    const PubKey &getPubKey() const;

    std::string toString() const;
    
    void signProposal(string, Proposal &); //throw ErrSignProposal
    void signVote(string, Vote &); //throw ErrSignProposal
};


#endif //TM_LIGHT_PRIVVALIDATOR_H
