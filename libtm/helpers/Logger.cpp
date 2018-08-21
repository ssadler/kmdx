//
// Created by utnso on 16/07/18.
//

#include "Logger.h"

const std::string Logger::channelTm = "overlaydb";

void Logger::logRound(int64_t h, int r) {
    clog(dev::VerbosityInfo, Logger::channelTm) << "@ height %i, round%i" << h << r;
}

void Logger::logRound(int64_t h) {
    clog(dev::VerbosityInfo, Logger::channelTm) << "@ height %i" << h;
}

