#include "timer.h"
#include <malloc.h>
#include <string.h>
#include <vector>

namespace utils
{
	/*��ʱ���ڵ�*/
	struct timer_node
	{
		timer_id id;				/*��ʱ��id��*/
		int interval;				/*��ʱ����ʱֵ*/
		int solt;					/*������ʱ����Ҫ��ʱ��Ƭ*/
		timer_expiry cb;			/*��ʱ���ص�����*/
		void *user_data;			/*��ʱ���������*/
		int len;					/*��ʱ�������������*/
		struct timer_node *next;
	};
	/*��ʱ������*/
	struct timer
	{
		int   time_solt;				/*��ʱ��ʱ��Ƭ��С*/
		int   solt_num;					/*��ʱ���������*/
		int   cur_solt;					/*��ʱ����ǰʱ��Ƭ��С*/
		int   solt_count;               /*��ʱ���ۼ�ʱ��Ƭ��С*/
		long  cur_time;					/*��ʱ����ǰʱ��*/
		long  pre_time;					/*��ʱ����һ��ʱ��*/
		get_time *gt;					/*��ʱ���õ���ǰʱ��*/
		struct timer_node **timer_list;
	} T;
	/*��ʼ����ʱ��*/
	/*num:��ʱ������*/
	/*solt:��ʱ��ʱ��Ƭ��С*/
	/*gt:��õ�ǰʱ�亯��*/
	/*����ֵ:0�ɹ�,-1ʧ��*/
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
	/*�������ж�ʱ��*/
	/*����ֵ:��*/
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
	/*���һ����ʱ��*/
	/*p:��ʱ���ڵ�ָ��*/
	/*����ֵ:��*/
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
	/*���һ����ʱ��*/
	/*id:��ʱ��id�û�ָ��,���򲻱�֤idΨһ*/
	/*interval:��ʱ����ʱֵ*/
	/*cb:��ʱ����ʱ�ص�����*/
	/*user_data:�������*/
	/*len:�����������*/
	/*����ֵ:0�ɹ�,-1ʧ��*/
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
	/*ɾ��һ����ʱ��*/
	/*id:��ʱ��id*/
	/*����ֵ:0�ɹ�,-1ʧ��*/
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

	/*ѭ����ʱ��*/
	/*����ֵ:��*/
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

