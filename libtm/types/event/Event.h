//
// Created by utnso on 17/07/18.
//

#ifndef TM_LIGHT_EVENT_H
#define TM_LIGHT_EVENT_H


#include <string>
#include "../Vote.h"
#include "../Heartbeat.h"

class Event {
    virtual std::string TypeString();
};

class EventCompleteProposal : public Event {
    virtual std::string TypeString() {
        return "EventCompleteProposal";
    }
};


class EventDupeout : public Event {
    virtual std::string TypeString() {
        return "EventDupeout";
    }
};

class EventFork : public Event {
    virtual std::string TypeString() {
        return "EventFork";
    }
};

class EventLock : public Event {
    virtual std::string TypeString() {
        return "EventLock";
    }
};

class EventNewBlock : public Event {
    virtual std::string TypeString() {
        return "EventNewBlock";
    }
};

class EventNewBlockHeader : public Event {
    virtual std::string TypeString() {
        return "EventNewBlockHeader";
    }
};

class EventNewRound : public Event {
    virtual std::string TypeString() {
        return "EventNewRound";
    }
};

class EventNewRoundStep : public Event {
    virtual std::string TypeString() {
        return "EventNewRoundStep";
    }
};

class EventPolka : public Event {
    virtual std::string TypeString() {
        return "EventPolka";
    }
};

class EventRebond : public Event {
    virtual std::string TypeString() {
        return "EventRebond";
    }
};

class EventRelock : public Event {
    virtual std::string TypeString() {
        return "EventRelock";
    }
};

class EventTimeoutPropose : public Event {
    virtual std::string TypeString() {
        return "EventTimeoutPropose";
    }
};

class EventTimeoutWait : public Event {
    virtual std::string TypeString() {
        return "EventTimeoutWait";
    }
};

class EventTx : public Event {
    virtual std::string TypeString() {
        return "EventTx";
    }
};

class EventUnbond : public Event {
    virtual std::string TypeString() {
        return "EventUnbond";
    }
};

class EventUnlock : public Event {
    virtual std::string TypeString() {
        return "EventUnlock";
    }
};

class EventVote : public Event {
    virtual std::string TypeString() {
        return "EventVote";
    }
};

class EventProposalHeartbeat : public Event {
    virtual std::string TypeString() {
        return "EventProposalHeartbeat";
    }
};

class EventDataVote : public Event {
    Vote vote;

    virtual std::string TypeString() {
        return "EventDataVote";
    }

public:
    EventDataVote(Vote v) : vote(v) {}
};

class EventDataProposalHeartbeat : public Event {
    Heartbeat *heartbeat;

    virtual std::string TypeString() {
        return "EventDataProposalHeartbeat";
    }

public :
    EventDataProposalHeartbeat(Heartbeat *h) {
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

    virtual std::string TypeString() {
        return "EventDataRoundState";
    }

    EventDataRoundState(int _height, int _round, std::string _step) {
        height = _height;
        round = _round;
        step = _step;
    }
};


#endif //TM_LIGHT_EVENT_H
