//
// Created by utnso on 17/07/18.
//

#ifndef TM_LIGHT_EVENT_H
#define TM_LIGHT_EVENT_H


#include <string>
#include "../Vote.h"
#include "../Heartbeat.h"

class Event {
    std::string typeToString;

    std::string getTypeToString();

public:
    Event(std::string typeString) : typeToString(typeString) {};
};

class EventCompleteProposal : public Event {
public:
    EventCompleteProposal() : Event("EventCompleteProposal") {}
};


class EventDupeout : public Event {
public:
    EventDupeout() : Event("EventDupeout") {}
};

class EventFork : public Event {
public:
    EventFork() : Event("EventFork") {}
};

class EventLock : public Event {
public:
    EventLock() : Event("EventLock") {}
};

class EventNewBlock : public Event {
public:
    EventNewBlock() : Event("EventNewBlock") {}
};

class EventNewBlockHeader : public Event {
public:
    EventNewBlockHeader() : Event("EventNewBlockHeader") {}
};

class EventNewRound : public Event {
public:
    EventNewRound() : Event("EventNewRound") {}
};

class EventNewRoundStep : public Event {
public:
    EventNewRoundStep() : Event("EventNewRoundStep") {}
};

class EventPolka : public Event {
public:
    EventPolka() : Event("EventPolka") {}
};

class EventRebond : public Event {
public:
    EventRebond() : Event("EventRebond") {}
};

class EventRelock : public Event {
public:
    EventRelock() : Event("EventRelock") {}
};

class EventTimeoutPropose : public Event {
public:
    EventTimeoutPropose() : Event("EventTimeoutPropose") {}
};

class EventTimeoutWait : public Event {
public:
    EventTimeoutWait() : Event("EventTimeoutWait") {}
};

class EventTx : public Event {
public:
    EventTx() : Event("EventTx") {}
};

class EventUnbond : public Event {
public:
    EventUnbond() : Event("EventUnbond") {}
};

class EventUnlock : public Event {
public:
    EventUnlock() : Event("EventUnlock") {}
};

class EventVote : public Event {
public:
    EventVote() : Event("EventVote") {}
};

class EventProposalHeartbeat : public Event {
public:
    EventProposalHeartbeat() : Event("EventProposalHeartbeat") {}
};

class EventDataVote : public Event {
    Vote vote;
public:
    EventDataVote(Vote v) : Event("EventDataVote"), vote(v) {}


};

class EventDataProposalHeartbeat : public Event {
    Heartbeat *heartbeat;


public :

    EventDataProposalHeartbeat(Heartbeat *h) : Event("EventDataProposalHeartbeat") {
        heartbeat = h;
    }
};

// NOTE: This goes into the replay WAL
class EventDataRoundState : public Event {
    int64_t height;
    int round;
    std::string step;
    // private, not exposed to websockets
    //TODO interface{} RoundState ;

    EventDataRoundState(int _height, int _round, std::string _step) : Event("EventDataRoundState") {
        height = _height;
        round = _round;
        step = _step;
    }
};


#endif //TM_LIGHT_EVENT_H
