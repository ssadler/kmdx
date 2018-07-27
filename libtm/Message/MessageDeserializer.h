//
// Created by thpn on 27/07/18.
//

#ifndef AYETH_MESSAGEDESERIALIZER_H
#define AYETH_MESSAGEDESERIALIZER_H

#include <libdevcore/RLP.h>
#include "Message.h"

class MessageDeserializer {
public:
    static shared_ptr<Message> deserialize(string _peerName, dev::RLP _message) {

        MessageType messageType = Message::typeFromString(_message[0].toString());
        shared_ptr<Message> message;
        PeerID pid = *deserializePeerId(_peerName).get();
        switch (messageType) {
            case MessageType::VoteType :
                return shared_ptr<Message>(new VoteMessage(pid, *deserializeVote(_message[1]).get()));
            case MessageType::ProposalType :
                return shared_ptr<Message>(new ProposalMessage(pid, *deserializeProposal(_message[1]).get()));
            case MessageType::TickerType :
                return shared_ptr<Message>(new TickerMessage(pid));
            case MessageType::QuitType :
                return shared_ptr<Message>(); //TODO
            case MessageType::BlockType :
                BlockID bid(_message[1].toVector<uint8_t>(0));
                return shared_ptr<Message>(
                        new BlockMessage(pid, bid, _message[2].toInt(), _message[3].toPositiveInt64(),
                                         deserializeBlock(_message[4])));


        }


    }

private : //TODO deserialization
    static shared_ptr<Block> deserializeBlock(dev::RLP rlp);

    static shared_ptr<Vote> deserializeVote(dev::RLP rlp);

    static shared_ptr<Proposal> deserializeProposal(dev::RLP rlp);

    static shared_ptr<PeerID> deserializePeerId(string str);

};


#endif //AYETH_MESSAGEDESERIALIZER_H
