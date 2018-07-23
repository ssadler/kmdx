//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_LOGGER_H
#define TM_LIGHT_LOGGER_H


#include <string>

class Logger {
public:

};

static void logInfo(std::string stuff...);

static void logError(std::string stuff...);

static void logDebug(std::string stuff...);

#endif //TM_LIGHT_LOGGER_H
