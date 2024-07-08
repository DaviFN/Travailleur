#include "Date.h"

#include "StringUtils.h"

Date::Date()
{
    const std::chrono::time_point now = std::chrono::system_clock::now();
    today = std::chrono::floor<std::chrono::days>(now);
}

int Date::getDayOfMonth() const
{
    return static_cast<unsigned>(today.day());
}

int Date::getMonth() const
{
    return static_cast<unsigned>(today.month());
}

int Date::getYear() const
{
    return static_cast<int>(today.year());
}

std::string Date::getDateFormatMMDDYYYY() const
{
    std::string monthString = std::to_string(getMonth());
    StringUtils::fillLeftWithCharactersUntilSizeIsAtLeast(monthString, '0', 2);
    std::string dayString = std::to_string(getDayOfMonth());
    StringUtils::fillLeftWithCharactersUntilSizeIsAtLeast(dayString, '0', 2);
    std::string yearString = std::to_string(getYear());
    StringUtils::fillLeftWithCharactersUntilSizeIsAtLeast(yearString, '0', 4);

    return monthString + "-" + dayString + "-" + yearString;
}

void Date::advanceDays(const int nDays)
{
    today = std::chrono::sys_days(today) + std::chrono::days{ nDays };
}

void Date::goBackDays(const int nDays)
{
    today = std::chrono::sys_days(today) - std::chrono::days{ nDays };
}