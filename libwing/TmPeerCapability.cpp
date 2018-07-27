//
// Created by thpn on 26/07/18.
//
#include "TmPeerCapability.h"

namespace dev
{
    namespace eth
    {
class BTFCPeerObserver : public BTFCPeerObserverFace {

    void onPeerMessage(std::shared_ptr<TmPeerCapability> _peer, RLP const& _message)  {

    }
};}}