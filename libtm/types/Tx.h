//
// Created by utnso on 12/07/18.
//

#ifndef TM_LIGHT_TX_H
#define TM_LIGHT_TX_H

#include <cstdint>
#include <string>
#include <zconf.h>
#include <vector>
#include "../state/State.h"

using namespace std;

class Tx {
public:
    vector<uint8_t> hash();

    string toString();

private:
    vector<uint8_t> bytes;
};

class Txs {
public:
    // Hash returns the simple Merkle root hash of the transactions.
    vector<uint8_t> hash();
    // Index returns the index of this transaction in the list, or -1 if not found
    // IndexByHash returns the index of this transaction hash in the list, or -1 if not found
    // Proof returns a simple merkle proof for this node.
    //TODO etc
private:
    vector<Tx> bytes;
};


#endif //TM_LIGHT_TX_H
