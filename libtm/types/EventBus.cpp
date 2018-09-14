//
// Created by utnso on 11/07/18.
//

#include "EventBus.h"



bool EventBus::publishEventVote(EventDataVote eventDataVote) {
    cout << eventDataVote.getTypeToString();
//FIXME unimplemented
    return true;
}

bool EventBus::publishEventUnlock(const EventDataRoundState eventDataRoundState) {
    cout << eventDataRoundState.getTypeToString();
//FIXME unimplemented
    return true;
}

void EventBus::publishEventCompleteProposal(const EventDataRoundState state) {
    cout << state.getTypeToString();
//FIXME unimplemented
}

void EventBus::publishEventPolka(const EventDataRoundState state) {
    cout << state.getTypeToString();
//FIXME unimplemented
}

void EventBus::publishEventRelock(const EventDataRoundState state) {
    cout << state.getTypeToString();
//FIXME unimplemented
}

void EventBus::publishEventLock(const EventDataRoundState state) {
    cout << state.getTypeToString();
//FIXME unimplemented
}

void EventBus::publishEventNewRoundStep(EventDataRoundState state) {
    cout << state.getTypeToString();
//FIXME unimplemented
}

void EventBus::publishEventNewRound(const EventDataRoundState state) {
    cout << state.getTypeToString();
//FIXME unimplemented
}

void EventBus::publishEventTimeoutPropose(const EventDataRoundState) {}

void EventBus::publishEventTimeoutWait(const EventDataRoundState) {}
