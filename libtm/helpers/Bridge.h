//
// Created by thpn on 25/07/18.
//

#ifndef AYETH_BRIDGE_H
#define AYETH_BRIDGE_H


#include <libtm/types/Block.h>

class Bridge {
public:
    shared_ptr<Block> getBlockToPropose();
};


#endif //AYETH_BRIDGE_H
