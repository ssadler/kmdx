//
// Created by Maru on 7/9/2018.
//

#include "Block.h"

Block::Block(int64_t _height, Commit _lastCommit) : height(_height), lastCommit(_lastCommit) {};

bool Block::hashesTo(HexBytes _hash) {
    return hash == _hash;
}

const HexBytes &Block::getBlockHash() const {
    return blockHash;
}

const HexBytes &Block::getAppHash() const {
    return appHash;
}

const HexBytes &Block::getHash() const {
    return hash;
}

const HexBytes &Block::getBytes() const {
    return bytes;
}

int64_t Block::getHeight() const {
    return height;
}

Block::Block() {
    //FIXME delete?
}
