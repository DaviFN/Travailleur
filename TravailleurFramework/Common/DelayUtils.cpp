#include "DelayUtils.h"

#include <thread>

void DelayUtils::performDelayInMs(const int msToDelay)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(msToDelay));
}