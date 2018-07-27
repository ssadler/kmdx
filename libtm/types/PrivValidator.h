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

    void signProposal(string, Proposal &);
};


#endif //TM_LIGHT_PRIVVALIDATOR_H
