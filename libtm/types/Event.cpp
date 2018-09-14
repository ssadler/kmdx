#include "Event.h"

Event::Event(std::string typeString) : typeToString(typeString) {}

std::string Event::getTypeToString() const {
    return typeToString;
}

EventDataRoundState::EventDataRoundState(height_t _height, int _round, std::string _step) : Event("EventDataRoundState") {
    height = _height;
    round = _round;
    step = _step;
}

