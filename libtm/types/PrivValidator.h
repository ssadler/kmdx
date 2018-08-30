//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_PRIVVALIDATOR_H
#define TM_LIGHT_PRIVVALIDATOR_H


#include "../../libdevcrypto/Common.h"
#include "../../libdevcore/SHA3.h"
#include "HexBytes.h"
#include "Proposal.h"

class PrivValidator {

protected:
    AddressTm addresstm;
    PrivKey privKey;
public:
    PrivValidator(const PrivKey _privKey);

    PrivValidator();

    AddressTm getAddress() const;

    const PubKey &getPubKey() const;

    std::string toString() const;
    
    void signProposal(string, Proposal &); //throw ErrSignProposal
    void signVote(string, Vote &); //throw ErrSignProposal
    const PrivKey &getPrivKey() const;
};


#endif //TM_LIGHT_PRIVVALIDATOR_H
