//
// Created by utnso on 17/07/18.
//

#include <err.h>
#include "HexBytes.h"

#ifndef TM_LIGHT_HEARTBEAT_H
#define TM_LIGHT_HEARTBEAT_H

/** Heartbeat is a simple vote-like structure so validators can
* alert others that they are alive and waiting for transactions.
* Note: We aren't adding ",omitempty" to Heartbeat's
* json field tags because we always want the JSON
* representation to be in its canonical form. **/
class Heartbeat {
public:
    Heartbeat(const AddressTm &validatorAddress, int validatorIndex, height_t height, int round, int sequence,
              const SignatureTm &signature);

private:
    friend class MockPV;

    AddressTm validatorAddress;
    int validatorIndex;
    height_t height;
    int round;
    int sequence;
    SignatureTm signature;

public:
    const HexBytes signBytes(std::string blockChainId) const;
};


#endif //TM_LIGHT_HEARTBEAT_H

