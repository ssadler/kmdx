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
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>

using byte = uint8_t;
using bytes = std::vector<byte>;
using height_t = uint64_t;

//TODO
struct HexBytes : public bytes {
private :


public:
    const HexBytes hash() const;

    HexBytes();

    HexBytes(std::string);

    explicit HexBytes(const std::vector<byte> b) : bytes(b) {}

    std::string toString() const;

    bool operator==(const HexBytes &other) const;

    static HexBytes random(int length);

    const static HexBytes fromRLP(dev::RLP r);

    const dev::RLPStream toRLP() const;

};

using AddressTm = HexBytes;
using SignatureTm = HexBytes;
using P2PID = HexBytes;

// BlockID defines the unique ID of a block as its Hash and its PartSetHeader
struct BlockID {
    HexBytes bytes;
    HexBytes hash;

    friend class VoteSetTest;

public:
    explicit BlockID(std::vector<uint8_t> _bytes);//FIXME rmv

    explicit BlockID(HexBytes _bytes);

    BlockID();

    std::string toString() const;

//    static BlockID fromRLP(dev::RLP &r);

    static BlockID fromRLP(dev::RLP r);

    dev::RLPStream toRLP();

    bool isEmpty() const;

    // Key returns a machine-readable string representation of the BlockID
    std::string key() const;

    HexBytes getBytes() const;

    HexBytes getHash() const;

    bool operator==(const BlockID &other) const;

};


class PubKey {
    AddressTm addresstm;
    dev::Public key;
public:
    PubKey(dev::Public _key, AddressTm _addresstm);

    explicit PubKey(AddressTm _address);

    explicit PubKey(dev::Secret);

    AddressTm getAddress() const;

    std::string toString() const;

    bool verifyBytes(HexBytes signBytes, SignatureTm signature) const;

    bool operator==(const PubKey &other);

};

class PrivKey {
    dev::Secret key;
    AddressTm address;
    PubKey pubKey;
public:
    PrivKey(AddressTm _address, HexBytes _pubKey);

    PrivKey();

    PrivKey(dev::Secret);

    AddressTm getAddress() const;

    void signBytes(HexBytes) const;

    HexBytes sign(HexBytes) const;

    const PubKey &getPubKey() const;

    std::string toString() const;

    bool verifyBytes(HexBytes signBytes, SignatureTm signature) const;

    const dev::Secret getKey() const;
};

#endif //TM_LIGHT_HEXBYTES_H
