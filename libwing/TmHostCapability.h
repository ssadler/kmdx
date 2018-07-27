//
// Created by thpn on 26/07/18.
//

#ifndef AYETH_BFTCONSENSUSHOSTCAPABILITY_H
#define AYETH_BFTCONSENSUSHOSTCAPABILITY_H
#include "TmPeerCapability.h"

#include <libdevcore/Worker.h>

namespace dev
{
    namespace eth {

        class BFTCHostCapability : public p2p::HostCapability<BFTCHostCapability>, Worker {

        };
    }}

#endif //AYETH_BFTCONSENSUSHOSTCAPABILITY_H
