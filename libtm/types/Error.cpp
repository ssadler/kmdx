//
// Created by utnso on 16/07/18.
//


#include "Error.h"

Error::Error(const string description) : description(description) {}

const string &Error::getDescription() const {
    return description;
}

