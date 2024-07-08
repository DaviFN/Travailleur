#include "DebugTimer.h"

DebugTimer::DebugTimer(std::string timerName, int timerType)
{
    this->timerType = timerType;
    this->name = timerName;
    this->hasBeenMeasured = false;
    time_constructed = std::chrono::steady_clock::now();
}

DebugTimer::~DebugTimer()
{
    if(hasBeenMeasured)
    {
        return;
    }
    auto time_destructed = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_difference = time_destructed - time_constructed;
    double duration = 1000 * time_difference.count();
    if(this->timerType == TIMER_TYPE_MB)
    {
        char msg[2048];
        sprintf(msg, "DebugTimer %s: %lf ms", this->name.c_str(), duration);
        dbgMsg(msg);
    }
    if(this->timerType == TIMER_TYPE_PRINTF)
    {
        printf("DebugTimer %s: %lf ms", this->name.c_str(), duration);
    }
    if(this->timerType == TIMER_TYPE_LOG)
    {
        LOG(DEBUG) << "DebugTimer" << this->name << ": " << duration << " ms";
    }
}

void DebugTimer::measure()
{
    auto time_measured = std::chrono::steady_clock::now();
    hasBeenMeasured = true;
    std::chrono::duration<double> time_difference = time_measured - time_constructed;
    double duration = 1000 * time_difference.count();
    if(this->timerType == TIMER_TYPE_MB)
    {
        char msg[2048];
        sprintf(msg, "DebugTimer %s: %lf ms", this->name.c_str(), duration);
        dbgMsg(msg);
    }
    if(this->timerType == TIMER_TYPE_PRINTF)
    {
        printf("DebugTimer %s: %lf ms\n", this->name.c_str(), duration);
    }
    if(this->timerType == TIMER_TYPE_LOG)
    {
        LOG(DEBUG) << "DebugTimer" << this->name << ": " << duration << " ms";
    }
}