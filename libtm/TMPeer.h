#pragma once

#include <mutex>
#include <array>
#include <memory>
#include <utility>

#include <libdevcore/RLP.h>
#include <libdevcore/Guards.h>
#include <libwingcore/Common.h>
#include <libp2p/Capability.h>

namespace dev
{
namespace tm
{

class TMPeer: public p2p::Capability
{
public:
	/// Basic constructor.
	TMPeer(std::shared_ptr<p2p::SessionFace> _s, p2p::HostCapabilityFace* _h, unsigned _i, p2p::CapDesc const& _cap);

    static std::string name() { return "tm"; }
    static u256 version() { return u256(0); }
};

}
}
