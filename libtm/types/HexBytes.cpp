//
// Created by thpn on 08/08/18.
//

#include "HexBytes.h"

HexBytes::HexBytes() {};

std::string HexBytes::toString() const {
    return "";//TODO
}

const HexBytes &BlockID::getBites() const {
    return bites;
}

const HexBytes &BlockID::getHash() const {
    return hash;
}

bool HexBytes::operator==(const HexBytes &other) const {
    if (other.size() != this->size()) return false;
    for (uint i = 0; i < size(); i++) {
        if (this->at(i) != other[i]) return false;
    }
    return true;
}

bool BlockID::operator==(const BlockID &other) const {
    return this->bites == other.getBites();
}

bool BlockID::isEmpty() const {
    return bites.empty() && this->hash.empty();
}

BlockID::BlockID() {};

BlockID::BlockID(std::vector<uint8_t> _bites) : bites(_bites) {};

Address Pubkey::getAddress() const {
    return address;
}

bool Pubkey::verifyBytes(HexBytes signBytes, Signature signature) const {
    return signBytes == signature; //TODO
}

