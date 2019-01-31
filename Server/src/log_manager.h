#ifndef LOG_MANAGER_H_
#define LOG_MANAGER_H_


#define MAX_TRACE_UIN_NUM 10

struct tagLogCfg
{	
	int TraceLogFlag;
    int DebugLogFlag;
	int WarnLogFlag;

	int MsgLogFlag;

	int OneFileLogCount;

	int AllTraceFlag;
	int TraceNum;
	unsigned int Traceuins[MAX_TRACE_UIN_NUM];
};

extern tagLogCfg g_LogCfg;

#endif
