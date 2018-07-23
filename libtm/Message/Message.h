//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_MESSAGE_H
#define TM_LIGHT_MESSAGE_H

class PeerID : public HexBytes {
};

class Message {
    PeerID peerID;
public:
    const PeerID &getPeerID() const {
        return peerID;
    }
};


#endif //TM_LIGHT_MESSAGE_H
