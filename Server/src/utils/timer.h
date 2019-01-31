#ifndef _UTIL_TIMER_H__
#define _UTIL_TIMER_H__
#include <functional>

namespace utils
{
	typedef unsigned long long timer_id;
	/*��ʱ����ʱ����*/
	typedef std::function<bool(timer_id id, void* user_data, int len)> timer_expiry;
	/*��õ�ǰʱ��*/
	typedef long get_time(void);

	/*��ʼ����ʱ��*/
	int init_timer(int num, int solt, get_time *gt);
	/*�������ж�ʱ��*/
	void destroy_timer();
	/*����һ����ʱ��*/
	timer_id add_timer(int interval, const timer_expiry& cb, void *user_data = NULL, int len = 0);
	/*ɾ��һ����ʱ��*/
	int delete_timer(timer_id id);
	/*���ж�ʱ��*/
	void timer_runonce();
}

#endif //_UTIL_TIMER_H__
