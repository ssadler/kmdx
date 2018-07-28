#pragma once

#include <libdevcore/Worker.h>
#include <libwingcore/Common.h>
#include <libp2p/Common.h>
#include <libwing/BlockChain.h>
#include "TMPeer.h"


namespace dev
{

namespace wing
{

/**
 * @brief The TMHost class
 * @warning None of this is thread-safe. You have been warned.
 * @doWork Syncs to peers and sends new blocks and transactions.
 */
class TMHost: public p2p::HostCapability<TMPeer>, Worker
{
public:
    /// Start server, but don't listen.
    TMHost(eth::BlockChain const& _bc, u256 _networkId): m_bc(_bc), m_networkId(_networkId) {};

    /// Will block on network process events.
    ~TMHost();

    void setNetworkId(u256 _networkId) { m_networkId = _networkId; }

protected:
    std::shared_ptr<p2p::Capability> newPeerCapability(std::shared_ptr<p2p::SessionFace> const& _s, unsigned _idOffset, p2p::CapDesc const& _cap) override;

private:
    void doWork() override;

    void onStopping() override { stopWorking(); }

    eth::BlockChain const& m_bc;
    u256 m_networkId;
    
    Logger m_logger{createLogger(VerbosityDebug, "tm")};
};

}
}
