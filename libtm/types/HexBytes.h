//
// Created by utnso on 12/07/18.
//

#ifndef TM_LIGHT_HEXBYTES_H
#define TM_LIGHT_HEXBYTES_H

#include <cstdint>
#include <vector>
#include <string>


//TODO
struct HexBytes {
private :
    std::vector<uint8_t> bytes;

public:
    HexBytes hash();

    const std::string toString() const;
    bool operator==(const HexBytes &other) {
        if (other.bytes.size() != bytes.size()) return false;
        for (uint i = 0; i < bytes.size(); i++) {
            if (bytes[i] != other.bytes[i]) return false;
        }
        return true;
    }

    const std::vector<uint8_t> &getBites() const {
        return bytes;
    }
};

// BlockID defines the unique ID of a block as its Hash and its PartSetHeader
struct BlockID {
    HexBytes bites;
    HexBytes hash;
public:

    BlockID(std::vector<uint8_t> bites);

//TODO set the bites
    BlockID(int i);

    BlockID();

    bool isEmpty();

    const HexBytes &getBites() const;

    const HexBytes &getHash() const;
};

bool BlockID::isEmpty() {
    return bites.getBites().empty();
}

const HexBytes &BlockID::getBites() const {
    return bites;
}

const HexBytes &BlockID::getHash() const {
    return hash;
}

bool operator==(const HexBytes &one, const HexBytes &other) {
    return one.getBites() == other.getBites();
}

#endif //TM_LIGHT_HEXBYTES_H
