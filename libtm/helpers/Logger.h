//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_LOGGER_H
#define TM_LIGHT_LOGGER_H


#include <string>
#include <libdevcore/Log.h>

std::string channelTm = "overlaydb";
class Logger {
public:

};

void logInfo(std::string, std::string &...);

void logError(std::string &...);

void logDebug(std::string &...);

#endif //TM_LIGHT_LOGGER_H
