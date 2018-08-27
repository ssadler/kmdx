//
// Created by utnso on 11/07/18.
//

#include "EventSwitch.h"

void EventSwitch::start() {
    //FIXME
}

void EventSwitch::fireEvent(Event event, const Vote &vote) {
    if (event.getTypeToString() == "") cout << vote.toString(); //TODO unimplemented
}

void EventSwitch::fireEvent(Event event, RoundState &rs) {
    if (rs.getHeight() > 0) cout << event.getTypeToString(); //TODO unimplemented
}

