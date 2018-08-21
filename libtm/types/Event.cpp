#include "Event.h"

Event::Event(std::string typeString) : typeToString(typeString) {}

std::string Event::getTypeToString() const {
    return typeToString;
}
