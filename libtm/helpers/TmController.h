//
// Created by thpn on 01/08/18.
//

#ifndef AYETH_TMCONTROLLER_H
#define AYETH_TMCONTROLLER_H

#include <cstdint>
#include <vector>
#include "../types/Block.h"

using byte = uint8_t;
using bytes = std::vector<byte>;

class TmController {
public :
    int validateBlock(bytes b);
    // currentHeight returns the last known contiguous block height.
    height_t currentHeight();

    void append(bytes block, std::vector<SignatureTm> signatures);

//    void append(HexBytes block, std::vector<HexBytes> signatures);

    void append(HexBytes, std::vector<HexBytes, std::allocator<HexBytes> >);

    Block getBlockToPropose();

    std::unordered_map<string, int> getGenesisParams();;
};


#endif //AYETH_TMCONTROLLER_H
