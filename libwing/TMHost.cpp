#include <libdevcore/Worker.h>
#include <libwingcore/Common.h>
#include <libp2p/Common.h>
#include <libwing/BlockChain.h>
#include "TMHost.h"
#include "TMPeer.h"


namespace dev
{

namespace wing
{

    /// Will block on network process events.
TMHost::~TMHost()
{
    terminate();
}

std::shared_ptr<p2p::Capability> TMHost::newPeerCapability(std::shared_ptr<p2p::SessionFace> const& _s, unsigned _idOffset, p2p::CapDesc const& _cap)
{
    auto ret = HostCapability<TMPeer>::newPeerCapability(_s, _idOffset, _cap);

    // TODO: wtf?

    //auto cap = p2p::capabilityFromSession<TMPeer>(*_s, _cap.second);
    //assert(cap);
    //cap->init(c_WarpProtocolVersion, m_networkId, m_blockChain.details().totalDifficulty,
    //    m_blockChain.currentHash(), m_blockChain.genesisHash(), m_snapshot, m_peerObserver);

    return ret;
}

void TMHost::doWork()
{
    // TODO
}

}
}

