#include "SimpleTimer.h"

SimpleTimer::SimpleTimer()
{
    referenceTimePoint = std::chrono::steady_clock::now();
}

void SimpleTimer::restart()
{
    referenceTimePoint = std::chrono::steady_clock::now();
}

double SimpleTimer::getElapsedTime()
{
    auto timePointMeasured = std::chrono::steady_clock::now();
    std::chrono::duration<double> timeDifference = timePointMeasured - referenceTimePoint;
    double elapsedTime = 1000 * timeDifference.count();
    return elapsedTime;
}