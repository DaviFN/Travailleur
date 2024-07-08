#ifndef SIMPLE_TIMER_H
#define SIMPLE_TIMER_H

#include <chrono>

class SimpleTimer
{
    private:
    std::chrono::steady_clock::time_point referenceTimePoint;
    public:
    SimpleTimer();
    void restart();
    double getElapsedTime();
};

#endif