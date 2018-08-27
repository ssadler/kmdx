//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_EVENTSWITCH_H
#define TM_LIGHT_EVENTSWITCH_H


#include "../types/event/Event.h"
#include "../types/RoundState.h"

class EventSwitch {
    int whatever; //FIXME rmv

public:
    void fireEvent(Event event, const Vote &vote);

    void fireEvent(Event event, RoundState &rs);


    void start();

};


#endif //TM_LIGHT_EVENTSWITCH_H
