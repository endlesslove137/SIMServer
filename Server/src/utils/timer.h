#ifndef _UTIL_TIMER_H__
#define _UTIL_TIMER_H__
#include <functional>

namespace utils
{
	typedef unsigned long long timer_id;
	/*定时器超时函数*/
	typedef std::function<bool(timer_id id, void* user_data, int len)> timer_expiry;
	/*获得当前时间*/
	typedef long get_time(void);

	/*初始化定时器*/
	int init_timer(int num, int solt, get_time *gt);
	/*销毁所有定时器*/
	void destroy_timer();
	/*增加一个定时器*/
	timer_id add_timer(int interval, const timer_expiry& cb, void *user_data = NULL, int len = 0);
	/*删除一个定时器*/
	int delete_timer(timer_id id);
	/*运行定时器*/
	void timer_runonce();
}

#endif //_UTIL_TIMER_H__
