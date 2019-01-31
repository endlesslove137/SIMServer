#include "timer.h"
#include <malloc.h>
#include <string.h>
#include <vector>

namespace utils
{
	/*定时器节点*/
	struct timer_node
	{
		timer_id id;				/*定时器id号*/
		int interval;				/*定时器超时值*/
		int solt;					/*触发定时器需要的时间片*/
		timer_expiry cb;			/*定时器回调函数*/
		void *user_data;			/*定时器传入参数*/
		int len;					/*定时器传入参数长度*/
		struct timer_node *next;
	};
	/*定时器管理*/
	struct timer
	{
		int   time_solt;				/*定时器时间片大小*/
		int   solt_num;					/*定时器最大数量*/
		int   cur_solt;					/*定时器当前时间片大小*/
		int   solt_count;               /*定时器累计时间片大小*/
		long  cur_time;					/*定时器当前时间*/
		long  pre_time;					/*定时器上一个时间*/
		get_time *gt;					/*定时器得到当前时间*/
		struct timer_node **timer_list;
	} T;
	/*初始化定时器*/
	/*num:定时器个数*/
	/*solt:定时器时间片大小*/
	/*gt:获得当前时间函数*/
	/*返回值:0成功,-1失败*/
	int init_timer(int num, int solt, get_time *gt)
	{
		memset(&T, 0, sizeof(struct timer));
		T.gt = gt;
		T.timer_list = (struct timer_node **)malloc(num*sizeof(struct timer_node *));
		if (0 == T.timer_list)
		{
			return -1;
		}
		int i = 0;
		while (i < num)
		{
			T.timer_list[i] = 0;
			i++;
		}
		T.time_solt = solt;
		T.solt_num = num;
		return 0;
	}
	/*销毁所有定时器*/
	/*返回值:无*/
	void destroy_timer()
	{
		int i = 0;
		struct timer_node *p = 0, *q = 0;
		while (i < T.solt_num)
		{
			p = T.timer_list[i];
			if (p)
			{
				while (p)
				{
					q = p;
					p = p->next;
					free(q);
				}
			}
			i++;
		}
		free(T.timer_list);
		return;
	}
	/*添加一个定时器*/
	/*p:定时器节点指针*/
	/*返回值:无*/
	int add_timer(struct timer_node *p)
	{
		int index = (T.cur_solt + p->interval / T.time_solt) % T.solt_num;
		p->solt = T.solt_count + p->interval / T.time_solt;
		if (T.timer_list[index])
		{
			p->next = T.timer_list[index];
		}
		T.timer_list[index] = p;
		return index;
	}
	/*添加一个定时器*/
	/*id:定时器id用户指定,程序不保证id唯一*/
	/*interval:定时器超时值*/
	/*cb:定时器超时回调函数*/
	/*user_data:传入参数*/
	/*len:传入参数长度*/
	/*返回值:0成功,-1失败*/
	timer_id add_timer(int interval, const timer_expiry& cb, void *user_data, int len)
	{
		struct timer_node *p = (struct timer_node *)malloc(sizeof(struct timer_node));
		if (0 == p)
		{
			return 0;
		}

		static unsigned int s_NextTimerId = 0;
		s_NextTimerId++;

		memset(p, 0, sizeof(struct timer_node));
		p->cb = cb;
		p->id = 0;
		p->len = len;
		p->interval = interval;
		p->user_data = user_data;
		unsigned int nSlot = add_timer(p);

		p->id = ((timer_id)nSlot << 32) | s_NextTimerId;
		return p->id;
	}
	/*删除一个定时器*/
	/*id:定时器id*/
	/*返回值:0成功,-1失败*/
	int delete_timer(timer_id id)
	{
		unsigned int slot = id >> 32;
		if (slot >= (unsigned int)T.solt_num)
		{
			return -1;
		}

		struct timer_node *p = 0, *q = 0;
		q = p = T.timer_list[slot];
		while (p)
		{
			if (p->id == id)
			{
				struct timer_node *temp = p;

				if (p == T.timer_list[slot])
				{
					T.timer_list[slot] = p->next;
					q = p->next;
				}
				else
				{
					q->next = p->next;
				}
				p = p->next;

				free(temp);
				break;
			}
			else
			{
				q = p;
				p = p->next;
			}
		}
		return 0;
	}

	/*循环定时器*/
	/*返回值:无*/
	void timer_runonce()
	{
		long diff_;
		struct timer_node *p = 0, *q = 0;
		T.cur_time = T.gt();
		if (0 == T.pre_time)
		{
			T.pre_time = T.cur_time;
		}
		diff_ = (T.cur_time - T.pre_time) / T.time_solt;

		while (diff_ > 0)
		{
			diff_--;

			T.pre_time = T.cur_time;
			T.cur_solt += 1;
			T.solt_count += 1;
			if (T.cur_solt >= T.solt_num)
			{
				T.cur_solt = 0;
			}

			std::vector<struct timer_node*> time_out_vec;
			q = T.timer_list[T.cur_solt];
			while (q)
			{
				p = q;
				q = q->next;
				if (T.solt_count >= p->solt)
				{
					T.timer_list[T.cur_solt] = q;
					time_out_vec.push_back(p);
				}
			}

			for (std::size_t i = 0; i < time_out_vec.size(); i++)
			{
				struct timer_node* p = time_out_vec[i];
				if (p->cb(p->id, p->user_data, p->len))
				{
					p->next = 0;
					add_timer(p);
				}
				else
				{
					free(p);
				}
			}
		}
	}
}

