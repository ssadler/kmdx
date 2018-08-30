//
// Created by thpn on 08/08/18.
//

#include "HexBytes.h"

HexBytes::HexBytes() {};

std::string HexBytes::toString() const {
    if (this->empty()) return "";
    if (this->size() == 0) return "";
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

HexBytes::HexBytes(std::string str) {
    for (unsigned char c : str)
        push_back(c);
}

HexBytes HexBytes::random(int length) {
    std::string chars(
            "abcdef"
            "ABCDEF"
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

BlockID::BlockID() : bytes(HexBytes("")), hash(HexBytes("")) {};

std::string BlockID::toString() const {
    std::ostringstream out;
    out << "{block_id:{hash:" << getHash().toString() << "}}";
    return out.str();
}

BlockID::BlockID(std::vector<uint8_t> _bytes) : bytes(_bytes), hash(_bytes) {}; //FIXME hash

AddressTm PubKey::getAddress() const {
    return addresstm;
}

//input: signBytes, signature
bool PubKey::verifyBytes(HexBytes, SignatureTm) const {
    return true;
//    return signBytes == signature?true:true; //TODO
}

std::string PubKey::toString() const {
    return "{PubKey:" + getAddress().toString() + "}";
}

PubKey::PubKey(AddressTm _address) : addresstm(_address) {}

bool PubKey::operator==(const PubKey &other) {
    return this->addresstm == other.addresstm;
}

AddressTm PrivKey::getAddress() const {
    return address;
}

const PubKey &PrivKey::getPubKey() const {
    return pubKey;
}

HexBytes PrivKey::sign(HexBytes b) const {
    return b; //TODO
}

PrivKey::PrivKey(AddressTm _address, HexBytes _pubKey) : address(_address), pubKey(_pubKey),
                                                         key(HexBytes::random(32)) {}

std::string PrivKey::toString() const {
    std::ostringstream out;
    out << address.toString();
    out << pubKey.toString();
    return out.str();
}

const bytes &PrivKey::getKey() const {
    return key;
}

