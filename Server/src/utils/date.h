#pragma once

#include <string>
#include <time.h>

namespace utils
{
	time_t stringToTime(const std::string& date);
	std::string timeToString(const time_t& time);
}