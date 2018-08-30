//
// Created by thpn on 01/08/18.
//

#include "TmController.h"

void TmController::append(HexBytes block, std::vector<HexBytes, std::allocator<HexBytes> > signatures) {
    if (block.empty() || signatures.empty()) return;

}

Block TmController::getBlockToPropose() {
    Block bl;
    return bl;
}

int TmController::validateBlock(bytes b) {
    return int(b[0]);
    //TODO this will probably require some state information
}

void TmController::append(bytes block, std::vector<SignatureTm> signatures) {
    if (block.empty() && signatures.empty()) return;
}

height_t TmController::currentHeight() {
    return 0; //TODO unimplemented
}

std::unordered_map<string, int> TmController::getGenesisParams() {
    return std::unordered_map<string, int>();
}
