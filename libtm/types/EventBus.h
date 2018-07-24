//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_EVENTBUS_H
#define TM_LIGHT_EVENTBUS_H


#include "event/Event.h"

class EventBus {
public:
    bool publishEventVote(EventDataVote eventDataVote);

    bool publishEventUnlock(const EventDataRoundState eventDataRoundState);

    void publishEventCompleteProposal(const EventDataRoundState state);

    void publishEventPolka(const EventDataRoundState state);

    void publishEventRelock(const EventDataRoundState state);

    void publishEventLock(const EventDataRoundState state);

    void publishEventNewRoundStep(EventDataRoundState state);

    void publishEventNewRound(const EventDataRoundState state);
};


#endif //TM_LIGHT_EVENTBUS_H
