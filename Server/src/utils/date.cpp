#include "date.h"
#include <string>
using namespace std;
namespace utils
{
	time_t stringToTime(const std::string& date)
	{
// 		tm time;
// 		time.tm_year = std::stoi(date.substr(0, 4));
// 		time.tm_mon = stoi(date.substr(5, 2)) + 1;
// 		time.tm_mday = stoi(date.substr(8, 2));
// 		time.tm_hour = stoi(date.substr(11, 2));
// 		time.tm_min = stoi(date.substr(14, 2));
// 		time.tm_sec = stoi(date.substr(17, 2));
// 		return mktime(&time);

		tm tm_;
		int year, month, day, hour, minute, second;
		sscanf(date.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
		tm_.tm_year = year - 1900;
		tm_.tm_mon = month - 1;
		tm_.tm_mday = day;
		tm_.tm_hour = hour;
		tm_.tm_min = minute;
		tm_.tm_sec = second;
		tm_.tm_isdst = 0;
		time_t t_ = mktime(&tm_); //已经减了8个时区
		return t_; //秒时间
	}

	std::string timeToString(const time_t& time)
	{
		char s[32];
		strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", localtime(&time));
		return std::string(s);
	}
}
