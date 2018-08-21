//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_LOGGER_H
#define TM_LIGHT_LOGGER_H


#include <string>
#include <libdevcore/Log.h>

class Logger {
public:
    static void logRound(int64_t, int);

    static void logRound(int64_t);

    static const std::string channelTm;
};


void logInfo(std::string, std::string &...);

void logError(std::string &...);

void logDebug(std::string &...);

#endif //TM_LIGHT_LOGGER_H
