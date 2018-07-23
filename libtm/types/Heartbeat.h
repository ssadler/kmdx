//
// Created by utnso on 17/07/18.
//

#include <zconf.h>
#include <err.h>
#include "Signature.h"
#include "HexBytes.h"

#ifndef TM_LIGHT_HEARTBEAT_H
#define TM_LIGHT_HEARTBEAT_H

/** Heartbeat is a simple vote-like structure so validators can
* alert others that they are alive and waiting for transactions.
* Note: We aren't adding ",omitempty" to Heartbeat's
* json field tags because we always want the JSON
* representation to be in its canonical form. **/
class Heartbeat {
    HexBytes validatorAddress;
    int validatorIndex;
    int64_t height;
    int round;
    int sequence;
    Signature signature;
};


#endif //TM_LIGHT_HEARTBEAT_H

