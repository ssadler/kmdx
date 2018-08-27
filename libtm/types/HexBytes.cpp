//
// Created by thpn on 08/08/18.
//

#include "HexBytes.h"

HexBytes::HexBytes() {};

std::string HexBytes::toString() const {
    std::ostringstream out;
    for (byte b : *this) {
        out << b;
    }
    return out.str();
}

std::string BlockID::key() const {
    return "{BlockID:" + hash.toString() + "}";
}

HexBytes BlockID::getBytes() const {
    return bytes;
}

HexBytes BlockID::getHash() const {
    return hash; //TODO
}

bool HexBytes::operator==(const HexBytes &other) const {
    if (other.size() != this->size()) return false;
    for (uint i = 0; i < size(); i++) {
        if (this->at(i) != other[i]) return false;
    }
    return true;
}

HexBytes::HexBytes(std::string) {
    bytes(string);
}

HexBytes HexBytes::random(int length) {
    std::string chars(
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "1234567890"
    );
    std::srand(std::time(nullptr));
    bytes _bytes;
    for (int i = 0; i < length; ++i) {
        _bytes.push_back(chars[std::rand() % chars.size()]);
    }
    return HexBytes(_bytes);
}

bool BlockID::operator==(const BlockID &other) const {
    return this->hash == other.hash;
}

bool BlockID::isEmpty() const {
    return bytes.empty();
}

BlockID::BlockID() : bytes(HexBytes("NULL")), hash(HexBytes("NULL")) {};

std::string BlockID::toString() const {
    std::ostringstream out;
    out << "{block_id:{hash:" << getHash().toString() << "}}";
    return out.str();
}

BlockID::BlockID(std::vector<uint8_t> _bytes) : bytes(_bytes), hash(_bytes) {}; //FIXME hash

Address PubKey::getAddress() const {
    return addresstm;
}

//input: signBytes, signature
bool PubKey::verifyBytes(HexBytes, Signature) const {
    return true;
//    return signBytes == signature?true:true; //TODO
}

std::string PubKey::toString() const {
    return "{PubKey:" + getAddress().toString() + "}";
}

PubKey::PubKey(Address _address) : addresstm(_address) {}

Address PrivKey::getAddress() const {
    return addresstm;
}

const PubKey &PrivKey::getPubKey() const {
    return pubKey;
}

HexBytes PrivKey::sign(HexBytes b) const {
    return b; //TODO
}

PrivKey::PrivKey(Address _address, HexBytes _pubKey) : addresstm(_address), pubKey(_pubKey) {}

