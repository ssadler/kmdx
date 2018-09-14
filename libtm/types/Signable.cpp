//
// Created by thpn on 05/09/18.
//

#include "Signable.h"

HexBytes Signable::signBytes(std::string chainID) const {
    return HexBytes(chainID + toString()+chainID + toString()+chainID + toString()+chainID + toString()+chainID + toString()+chainID + toString()+chainID + toString());
//    return HexBytes(dev::sha3(chainID + toString()).asBytes());
}

void Signable::setSignature(const SignatureTm &_signature) {
    this->signature = _signature;
}

const SignatureTm &Signable::getSignature() const {
    return signature;
}

std::string Signable::toString() const {return "signable";}
