#include <libdevcore/RLP.h>
#include <libwingcore/Common.h>
#include <libp2p/Capability.h>
#include <libp2p/Session.h>
#include <libp2p/Host.h>
#include "TMPeer.h"


namespace dev
{
namespace wing
{

TMPeer::TMPeer(std::shared_ptr<p2p::SessionFace> _s, p2p::HostCapabilityFace* _h, unsigned _i, p2p::CapDesc const& _cap):
    Capability(_s, _h, _i)
{
    // TODO: ??
    (void)_cap;
    //m_peerCapabilityVersion(_cap.second)
    //session()->addNote("manners", isRude() ? "RUDE" : "nice");
};

bool TMPeer::interpret(unsigned _id, RLP const& _r)
{
    if (_id == 1 && &_r) return true;
    return true; // TODO
}
}
}
