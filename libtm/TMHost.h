#pragma once

#include <mutex>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <memory>
#include <utility>
#include <thread>

#include <libdevcore/Guards.h>
#include <libdevcore/Worker.h>
#include <libwingcore/Common.h>
#include <libp2p/Common.h>
#include <libdevcore/OverlayDB.h>
#include <libwingcore/BlockHeader.h>
#include <libwing/BlockChain.h>
#include "TMPeer.h"

namespace dev
{

namespace tm
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
    TMHost(eth::BlockChain const& _ch, u256 _networkId);

    /// Will block on network process events.
    virtual ~TMHost();

protected:
    std::shared_ptr<p2p::Capability> newPeerCapability(std::shared_ptr<p2p::SessionFace> const& _s, unsigned _idOffset, p2p::CapDesc const& _cap) override;

private:
    virtual void doWork() override;

    virtual void onStarting() override { startWorking(); }
    virtual void onStopping() override { stopWorking(); }

    eth::BlockChain const& m_chain;
    u256 m_networkId;
    
    Logger m_logger{createLogger(VerbosityDebug, "tm")};
};

}
}
