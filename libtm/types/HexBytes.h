//
// Created by utnso on 12/07/18.
//

#ifndef TM_LIGHT_HEXBYTES_H
#define TM_LIGHT_HEXBYTES_H

#include <cstdint>
#include <vector>
#include <string>
#include <random>
#include <libdevcore/RLP.h>
#include <boost/format.hpp>

using byte = uint8_t;
using bytes = std::vector<byte>;
using height_t = uint64_t;

//TODO
struct HexBytes : public bytes {
private :


public:
    HexBytes hash();

    HexBytes();

    HexBytes(std::string);

    explicit HexBytes(const std::vector<byte> b) : bytes(b) {}

    std::string toString() const;

    bool operator==(const HexBytes &other) const;

    static HexBytes random(int length);

};

//using Address = HexBytes;
using Address = HexBytes;
using Signature = HexBytes;
using P2PID = HexBytes;

// BlockID defines the unique ID of a block as its Hash and its PartSetHeader
struct BlockID {
    HexBytes bytes;
    HexBytes hash;

    friend class VoteSetTest;

public:
    explicit BlockID(std::vector<uint8_t> _bytes);

    BlockID();

    std::string toString() const;

    static BlockID fromRLP(dev::RLP &r);

    bool isEmpty() const;

    // Key returns a machine-readable string representation of the BlockID
    std::string key() const;

    HexBytes getBytes() const;

    HexBytes getHash() const;

    bool operator==(const BlockID &other) const;

};


class PubKey {
    Address addresstm;
public:

    explicit PubKey(Address _address);

    Address getAddress() const;

    std::string toString() const;

    bool verifyBytes(HexBytes signBytes, Signature signature) const;

};

class PrivKey {
    Address addresstm;
    PubKey pubKey;
public:

    PrivKey(Address _address, HexBytes _pubKey);

    Address getAddress() const;

    void signBytes(HexBytes) const;

    HexBytes sign(HexBytes) const;

    const PubKey &getPubKey() const;

    std::string toString() const;

    bool verifyBytes(HexBytes signBytes, Signature signature) const;

};

#endif //TM_LIGHT_HEXBYTES_H
