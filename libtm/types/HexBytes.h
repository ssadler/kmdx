//
// Created by utnso on 12/07/18.
//

#ifndef TM_LIGHT_HEXBYTES_H
#define TM_LIGHT_HEXBYTES_H

#include <cstdint>
#include <vector>
#include <string>
#include <libdevcore/RLP.h>

using byte = uint8_t;
using bytes = std::vector<byte>;


//TODO
struct HexBytes : std::vector<byte> {
private :


public:
    HexBytes hash();

    HexBytes();

    explicit HexBytes(const std::vector<byte> b) : bytes(b) {}

    std::string toString() const;

    bool operator==(const HexBytes &other) const;

};

using PeerID = HexBytes;
using Address = HexBytes;
using Signature = HexBytes;
using P2PID = HexBytes;

// BlockID defines the unique ID of a block as its Hash and its PartSetHeader
struct BlockID {
    HexBytes bites;
    HexBytes hash;
public:
    explicit BlockID(std::vector<uint8_t> _bites);

    BlockID();

    static BlockID fromRLP(dev::RLP &r);

    bool isEmpty() const;


    // Key returns a machine-readable string representation of the BlockID
    std::string key() const;

    const HexBytes &getBites() const;

    const HexBytes &getHash() const;

    bool operator==(const BlockID &other) const;

};


class Pubkey {
    Address address;
public:
    Address getAddress() const;

    std::string toString() const;

    bool verifyBytes(HexBytes signBytes, Signature signature) const;

    //TODO implement
};

#endif //TM_LIGHT_HEXBYTES_H
