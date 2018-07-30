//
// Created by utnso on 12/07/18.
//

#ifndef TM_LIGHT_SIGNATURE_H
#define TM_LIGHT_SIGNATURE_H
#include <libdevcore/RLP.h>

class Signature {
public:
    static Signature fromRLP(dev::RLP &r);
    dev::u256 toRLP();
};


#endif //TM_LIGHT_SIGNATURE_H
