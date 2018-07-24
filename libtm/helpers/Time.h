//
// Created by utnso on 19/07/18.
//

#ifndef TM_LIGHT_TIME_H
#define TM_LIGHT_TIME_H

#include <ctime>
#include <chrono>
#include <ratio>

using namespace std::chrono;

class Time {
    time_point<system_clock> time;
public:
    Time(const time_point<system_clock> &time);

    Time();

    bool after(time_point<system_clock> fun);

    /*static time_point<system_clock> now();*/

    int operator-(Time t);

    bool isZero();


    Time(duration<system_clock> duration);

    bool after(Time t);
};


#endif //TM_LIGHT_TIME_H
