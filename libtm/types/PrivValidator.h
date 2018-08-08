//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_PRIVVALIDATOR_H
#define TM_LIGHT_PRIVVALIDATOR_H


#include "HexBytes.h"
#include "Proposal.h"

class PrivValidator {
public:
    const HexBytes getAddress() const;

    std::string toString() const;
    

    void signProposal(string, Proposal &); //throw ErrSignProposal
    void signVote(string, Vote &); //throw ErrSignProposal
};


#endif //TM_LIGHT_PRIVVALIDATOR_H
