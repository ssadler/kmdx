//
// Created by thpn on 05/09/18.
//

#ifndef AYETH_SIGNABLE_H
#define AYETH_SIGNABLE_H


#include <string>
#include "HexBytes.h"

class Signable {

protected:
    SignatureTm signature;

public:
    HexBytes signBytes(std::string chainID) const;

    virtual std::string toString() const;

    void setSignature(const SignatureTm &_signature);

    const SignatureTm &getSignature() const;


};


#endif //AYETH_SIGNABLE_H
