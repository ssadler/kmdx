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

class EventCompleteProposal : Event {
    virtual std::string TypeString() {
        return "EventCompleteProposal";
    }
};


class EventDupeout : Event {
    virtual std::string TypeString() {
        return "EventDupeout";
    }
};

class EventFork : Event {
    virtual std::string TypeString() {
        return "EventFork";
    }
};

class EventLock : Event {
    virtual std::string TypeString() {
        return "EventLock";
    }
};

class EventNewBlock : Event {
    virtual std::string TypeString() {
        return "EventNewBlock";
    }
};

class EventNewBlockHeader : Event {
    virtual std::string TypeString() {
        return "EventNewBlockHeader";
    }
};

class EventNewRound : Event {
    virtual std::string TypeString() {
        return "EventNewRound";
    }
};

class EventNewRoundStep : Event {
    virtual std::string TypeString() {
        return "EventNewRoundStep";
    }
};

class EventPolka : Event {
    virtual std::string TypeString() {
        return "EventPolka";
    }
};

class EventRebond : Event {
    virtual std::string TypeString() {
        return "EventRebond";
    }
};

class EventRelock : Event {
    virtual std::string TypeString() {
        return "EventRelock";
    }
};

class EventTimeoutPropose : Event {
    virtual std::string TypeString() {
        return "EventTimeoutPropose";
    }
};

class EventTimeoutWait : Event {
    virtual std::string TypeString() {
        return "EventTimeoutWait";
    }
};

class EventTx : Event {
    virtual std::string TypeString() {
        return "EventTx";
    }
};

class EventUnbond : Event {
    virtual std::string TypeString() {
        return "EventUnbond";
    }
};

class EventUnlock : Event {
    virtual std::string TypeString() {
        return "EventUnlock";
    }
};

class EventVote : Event {
    virtual std::string TypeString() {
        return "EventVote";
    }
};

class EventProposalHeartbeat : Event {
    virtual std::string TypeString() {
        return "EventProposalHeartbeat";
    }
};

class EventDataVote : Event {
    Vote vote;

    virtual std::string TypeString() {
        return "EventDataVote";
    }

public:
    EventDataVote(Vote v) {
        vote = v;
    }
};

class EventDataProposalHeartbeat : Event {
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
class EventDataRoundState : Event {
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
