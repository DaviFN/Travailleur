#ifndef DEBUG_TIMER_H
#define DEBUG_TIMER_H

#include <chrono>
#include <string>
#include "TravailleurFramework\Common\common_functions\common_functions.h"
#include "TravailleurFramework\3rdparty\EasyLogging\easylogging++.h"

#define TIMER_TYPE_MB 0
#define TIMER_TYPE_PRINTF 1
#define TIMER_TYPE_LOG 2

class DebugTimer
{
    private:
    std::chrono::_V2::steady_clock::time_point time_constructed;
    int timerType;
    std::string name;
    bool hasBeenMeasured;
    public:
    DebugTimer(std::string timerName, int timerType);
    ~DebugTimer();
    void measure();
};

#endif