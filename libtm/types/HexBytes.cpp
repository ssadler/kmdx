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

const HexBytes HexBytes::hash() const { //TODO
    std::ostringstream output;
    for (byte b : *this)
        output << b;
    return HexBytes(output.str());
}

const dev::RLPStream HexBytes::toRLP() const {
    dev::RLPStream out;
    out.appendList(1);
    out << *this;
    return out;
}

const HexBytes HexBytes::fromRLP(dev::RLP r) {
    return HexBytes(r[0].toBytes());
}

HexBytes::HexBytes(dev::h256 hash) {
    for (auto a : hash){
        push_back(a);
    }
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

BlockID::BlockID(std::vector<uint8_t> _bytes) : bytes(_bytes), hash(_bytes) {
//FIXME hash
}

BlockID::BlockID(HexBytes _bytes) : bytes(_bytes), hash(_bytes) {
//FIXME hash
}

BlockID BlockID::fromRLP(dev::RLP r) {
    return BlockID(r[0].toBytes());
}

AddressTm PubKey::getAddress() const {
    return addresstm;
}

//input: signBytes, signature
bool PubKey::verifyBytes(HexBytes, SignatureTm ) const {
//    return(dev::verify(dev::Public(this->key), dev::Signature(signature),  dev::sha3(signBytes.toString())));
    return true;

}

std::string PubKey::toString() const {
    return "{PubKey:" + getAddress().toString() + "}";
}

PubKey::PubKey(AddressTm _address) : addresstm(_address) {}

bool PubKey::operator==(const PubKey &other) {
    return this->addresstm == other.addresstm;
}

PubKey::PubKey(dev::Public _key, AddressTm _addresstm) : addresstm(_addresstm), key(_key) {}

PubKey::PubKey(dev::Secret secret) : addresstm(dev::toAddress(secret).asBytes()), key(dev::toPublic(secret)) {};

AddressTm PrivKey::getAddress() const {
    return address;
}

const PubKey &PrivKey::getPubKey() const {
    return pubKey;
}

HexBytes PrivKey::sign(HexBytes b) const {
    return SignatureTm(dev::sign(key, dev::sha3(b)).asBytes());
}

std::string PrivKey::toString() const {
    std::ostringstream out;
    out << address.toString();
    out << pubKey.toString();
    return out.str();
}

const dev::Secret PrivKey::getKey() const {
    return key;
}

PrivKey::PrivKey(AddressTm _address, HexBytes _pubKey) : key(HexBytes::random(32)), address(_address),
                                                         pubKey(_pubKey) {}

PrivKey::PrivKey(dev::Secret secret) :
        key(secret), address(dev::toAddress(secret).asBytes()), pubKey(dev::toPublic(secret), address) {
    //TODO copy secret key -  getting null!!!
}

PrivKey::PrivKey() : PrivKey(dev::Secret()) {}

dev::RLPStream BlockID::toRLP() {
    dev::RLPStream output;
    output.appendList(1);
    output << bytes;
    return output;
}
