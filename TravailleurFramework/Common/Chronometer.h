#ifndef CHRONOMETER_H
#define CHRONOMETER_H

#include <chrono>
#include <cmath>
#include <cstdio>
#include <string>

#define RATIO_NANOSECOND_MICROSECOND 1000
#define RATIO_MICROSECOND_MILLISECOND 1000
#define RATIO_SECOND_MILLISECOND 1000
#define RATIO_MINUTE_SECOND 60
#define RATIO_HOUR_MINUTE 60
#define RATIO_DAY_HOUR 24

class Chronometer
{
    public:
    enum class Unit
    {
        Nanoseconds,
        Microseconds,
        Milliseconds,
        Seconds,
        Minutes,
        Hours,
        Days
    };

    Chronometer()
    {
        restart();
    }

    ~Chronometer() = default;

    void restart()
    {
        running = true;
        referenceTimePoint = std::chrono::steady_clock::now();
    }

    void stop()
    {
        if (running) {
            stopTimePoint = std::chrono::steady_clock::now();
            running = false;
        }
    }

    void start()
    {
        if (!running) {
            referenceTimePoint = std::chrono::steady_clock::now() - (stopTimePoint - referenceTimePoint);
            running = true;
        }
    }

    void reset()
    {
        referenceTimePoint = std::chrono::steady_clock::now();
        stopTimePoint = referenceTimePoint;
        running = false;
    }

    bool isRunning() const
    {
        return running;
    }

        double TimeElapsed(const Unit unit = Unit::Seconds) const
    {
        const std::chrono::time_point<std::chrono::steady_clock> endTimePoint = running ? std::chrono::steady_clock::now() : stopTimePoint;
        const std::chrono::duration<double> duration = endTimePoint - referenceTimePoint;
        const double timeElapsedInSeconds = duration.count();
        double timeElapsedInChosenUnit;
        switch (unit) {
        case Unit::Nanoseconds:
            timeElapsedInChosenUnit = timeElapsedInSeconds * RATIO_SECOND_MILLISECOND * RATIO_MICROSECOND_MILLISECOND * RATIO_NANOSECOND_MICROSECOND;
            break;
        case Unit::Microseconds:
            timeElapsedInChosenUnit = timeElapsedInSeconds * RATIO_SECOND_MILLISECOND * RATIO_MICROSECOND_MILLISECOND;
            break;
        case Unit::Milliseconds:
            timeElapsedInChosenUnit = timeElapsedInSeconds * RATIO_SECOND_MILLISECOND;
            break;
        case Unit::Minutes:
            timeElapsedInChosenUnit = timeElapsedInSeconds / RATIO_MINUTE_SECOND;
            break;
        case Unit::Hours:
            timeElapsedInChosenUnit = timeElapsedInSeconds / (RATIO_MINUTE_SECOND * RATIO_HOUR_MINUTE);
            break;
        case Unit::Days:
            timeElapsedInChosenUnit = timeElapsedInSeconds / (RATIO_MINUTE_SECOND * RATIO_HOUR_MINUTE * RATIO_DAY_HOUR);
            break;
        default:
            timeElapsedInChosenUnit = timeElapsedInSeconds;
        }
        return timeElapsedInChosenUnit;
    }

    std::string TimeElapsedAsString() const
    {
        size_t seconds = static_cast<size_t>(floor(TimeElapsed()));
        constexpr size_t ratioHourSecond = RATIO_MINUTE_SECOND * RATIO_HOUR_MINUTE;
        if (seconds >= 100 * ratioHourSecond) {
            return "99:59:59";
        }
        const size_t hours = seconds / ratioHourSecond;
        seconds = seconds % ratioHourSecond;
        const size_t minutes = seconds / RATIO_MINUTE_SECOND;
        seconds = seconds % RATIO_MINUTE_SECOND;

        char timeCStr[9];
        snprintf(timeCStr, 9, "%02zu:%02zu:%02zu", hours, minutes, seconds);

        return timeCStr;
    }

    private:
    std::chrono::time_point<std::chrono::steady_clock> referenceTimePoint;
    std::chrono::time_point<std::chrono::steady_clock> stopTimePoint;
    bool running = true;
};

#endif