//
// Created by thpn on 26/07/18.
//

#pragma once

#include "CommonNet.h"

#include <libdevcore/Common.h>
#include <libp2p/Capability.h>
class BTFCPeerCapability;
namespace dev
{
    namespace eth
    {
        const unsigned c_BTFCProtocolVersion = 1;

        enum BFTCSubprotocolPacketType : byte
        {
            Message = 0x00,
            BFTCSubprotocolPacketType
        };

        class TmPeerObserverFace
        {
        public:
            virtual ~PeerObserverFace() {}

            virtual void onPeerMessage(std::shared_ptr<BTFCPeerCapability> _peer, RLP const& _message) = 0;

        };

        class BTFCPeerCapability : public p2p::Capability
        {
        public:
            BTFCPeerCapability(std::shared_ptr<p2p::SessionFace> _s, p2p::HostCapabilityFace* _h,
                               unsigned _i, p2p::CapDesc const& _cap);

            static std::string name() { return "par"; }

            static u256 version() { return c_BTFCProtocolVersion; }

            static unsigned messageCount() { return BFTCSubprotocolPacketType; }

            void init(unsigned _hostProtocolVersion, u256 _hostNetworkId, h256 _chainCurrentHash, //THIS?
                    h256 _chainGenesisHash, std::weak_ptr<PeerObserverFace> _observer);

            /// Validates whether peer is able to communicate with the host, disables peer if not
            bool validateStatus(h256 const& _genesisHash, std::vector<unsigned> const& _protocolVersions,
                                u256 const& _networkId);

            void requestStatus(unsigned _hostProtocolVersion, u256 const& _hostNetworkId,
                               h256 const& _chainCurrentHash,
                               h256 const& _chainGenesisHash);
            //void requestBlockHeaders(unsigned _startNumber, unsigned _count, unsigned _skip, bool _reverse);
            //void requestManifest();
            void requestData(h256 const& _chunkHash);

            /// Host runs this periodically to check up on the peer.
            void tick();

            //u256 snapshotNumber() const { return m_snapshotNumber; }

            using p2p::Capability::disable;

        private:
            using p2p::Capability::sealAndSend;

            bool interpret(unsigned _id, RLP const& _r) override;

            void setAsking(Asking _a);

            void setIdle() { setAsking(Asking::Nothing); }

            /// Peer's protocol version.
            unsigned m_protocolVersion = 0;

            /// Peer's network id.
            u256 m_networkId;

            /// What, if anything, we last asked the other peer for.
            Asking m_asking = Asking::Nothing;
            /// When we asked for it. Allows a time out.
            std::atomic<time_t> m_lastAsk;

            /// These are determined through either a Status message.
            h256 m_latestHash;       ///< Peer's latest block's hash.
            h256 m_genesisHash;      ///< Peer's genesis hash

            std::weak_ptr<PeerObserverFace> m_observer;
        };

    }  // namespace eth
}  // namespace dev
