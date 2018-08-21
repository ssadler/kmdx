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
    return "{BlockID:" + bytes.toString() + "}";
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
            "!@#$%^&*()"
            "`~-_=+[{]{\\|;:'\",<.>/? ");
    std::srand(std::time(nullptr));
    bytes _bytes;
    for (int i = 0; i < length; ++i) {
        _bytes.push_back(chars[std::rand() % chars.size()]);
    }
    return HexBytes(_bytes);
}

bool BlockID::operator==(const BlockID &other) const {
    return this->bytes == other.getBytes();
}

bool BlockID::isEmpty() const {
    return bytes.empty();
}

BlockID::BlockID() {};

std::string BlockID::toString() const {
    std::ostringstream out;
    out << "{block_id:{hash:" << getHash().toString() << "}}";
    return out.str();
}

BlockID::BlockID(std::vector<uint8_t> _bytes) : bytes(_bytes), hash(_bytes) {}; //FIXME hash

Address PubKey::getAddress() const {
    return address;
}

bool PubKey::verifyBytes(HexBytes signBytes, Signature signature) const {
    return signBytes == signature; //TODO
}

std::string PubKey::toString() const {
    return "{PubKey:" + getAddress().toString() + "}";
}

PubKey::PubKey(Address _address) : address(_address) {}

Address PrivKey::getAddress() const {
    return address;
}

const PubKey &PrivKey::getPubKey() const {
    return pubKey;
}

HexBytes PrivKey::sign(HexBytes b) const {
    return b; //TODO
}

PrivKey::PrivKey(HexBytes _address, HexBytes _pubKey) : address(_address), pubKey(_pubKey) {}

