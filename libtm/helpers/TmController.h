//
// Created by thpn on 01/08/18.
//

#ifndef AYETH_TMCONTROLLER_H
#define AYETH_TMCONTROLLER_H

#include <cstdint>
#include <vector>
#include <libtm/types/Block.h>

using byte = uint8_t;
using bytes = std::vector<byte>;

class TmController {
public :
    int validateBlock(bytes b); //TODO this will probably require some state information
    // currentHeight returns the last known contiguous block height.
    int64_t currentHeight();

    void append(bytes block, bytes signatures);

    Block getBlockToPropose();

    std::unordered_map<string, int> getGenesisParams();
};


#endif //AYETH_TMCONTROLLER_H
