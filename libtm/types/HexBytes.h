//
// Created by utnso on 12/07/18.
//

#ifndef TM_LIGHT_HEXBYTES_H
#define TM_LIGHT_HEXBYTES_H

#include <cstdint>
#include <vector>


//TODO
struct HexBytes {
private :
    std::vector<uint8_t> bites;

public:
    HexBytes hash();

    bool operator==(const HexBytes &other) {
        if (other.bites.size() != bites.size()) return false;
        for (size_t i = 0; i < bites.size(); i++) {
            if (bites[i] != other.bites[i]) return false;
        }
        return true;
    }

    const std::vector<uint8_t> &getBites() const {
        return bites;
    }
};

// BlockID defines the unique ID of a block as its Hash and its PartSetHeader
struct BlockID {
    HexBytes bites;
public:
    HexBytes hash(); //TODO check is this a function or a static?
    bool isEmpty();
};

bool BlockID::isEmpty() { //TODO fugly
    return bites.getBites().size() == 0;
}

bool operator==(const HexBytes &one, const HexBytes &other) {
    return one.getBites() == other.getBites();
}

#endif //TM_LIGHT_HEXBYTES_H
