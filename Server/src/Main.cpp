#include "base.h"

#include "utils/timer.h"
#include "Network/Network.h"
#include "client/ClientManager.h"
#include "database/Database.h"
#include "GameConst.h"
#include "utils/date.h"


//#define Local_test
#ifdef Local_test
	#define DBUser "root"
	#define DBPass "123456"
	#define DBPort 3306
	#define DBName "sim"
	#define DBHost "192.168.11.77"
#else
	#define DBUser "wymaster"
	#define DBPass "$$$newpassword2009!!!"
	#define DBPort 3306
	#define DBName "sim"
	#define DBHost "221.228.78.207"
#endif Local_test




#define SHost "0.0.0.0"
#define SPort 12400
#define SHHost "0.0.0.0"
#define SHPort 11300

#include <tchar.h>
#include <Dbghelp.h>
#pragma auto_inline (off)
#pragma comment( lib, "DbgHelp" )

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)    //dump
{
	//HANDLE lhDumpFile = CreateFile(_T("DumpFile.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE lhDumpFile = CreateFile(_T("DumpFile.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;

	loExceptionInfo.ExceptionPointers = ExceptionInfo;

	loExceptionInfo.ThreadId = GetCurrentThreadId();

	loExceptionInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), lhDumpFile, MiniDumpNormal, &loExceptionInfo, NULL, NULL);

	CloseHandle(lhDumpFile);

	return EXCEPTION_EXECUTE_HANDLER;
}

long timer_get_time(void)
{
	return clock();
}

bool check_logtable_timer_expiry(utils::timer_id id, void *user_data, int len)
{
	static int s_Amount = 0;
	s_Amount++;
//	printf("check_logtable_timer_expiry\n");
	printf("%s\n", utils::timeToString(time(NULL)).c_str());

	
	//if (s_Amount >= 3)
//	{
//		return false;
//	}
	return true;
}

DBRequestQueue g_DBQueue;

void initDatabase()
{
	MYSQL_PARAM db_param;
	db_param.nPort = DBPort;
	db_param.strDefaultDB = DBName;
	db_param.strHost = DBHost;
	db_param.strLogin = DBUser;
	db_param.strPasswd = DBPass;

	Database db;
	db.SetDBConfig(db_param);

	if (!db.Logon())
	{
		printf("DB connect faild\n");
	}

	g_DBQueue.Init(3, db_param);
}

void STDCALL InitDefMsgOut()
{
// 	InitializeCriticalSection(&g_OutputMsgLock);
// 	g_lpOutputMsg = StdDefOutputMsg;

	//设置CTYPE为本地代码页，防止出现无法打印UNICODE 255之后的字符的问题。
	char sACP[16];
	sprintf(sACP, ".%d", GetACP());
	setlocale(LC_CTYPE, sACP);
}

AsioNetwork g_network;
AsioNetwork g_httpServer;

int main()
{
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);   //add dump

#pragma message(“消息文本”)


//	DebugLog::instance()->set_log(LOG_TRACE, LOG_TYPE_NORMAL, ".", "Server");
	utils::init_timer(10000, 50, timer_get_time);

	//WRITE_ERR_LOG("LogTest %d", 1234); // ��־����
	//ThreadTest(); // �̲߳���
	//DBTest();  // DB����
	//utils::add_timer(1000, check_logtable_timer_expiry, NULL, 0); // ��ʱ������


//	utils::add_timer(3000, check_logtable_timer_expiry, NULL, 0); // ��ʱ������

	

	//��ʼ����ݿ�
	initDatabase();

	//uint16_t *unicode_data = NULL;
	//int unicode_size = 0;
	//unsigned char *utfdata;
	//unsigned long data_size;
	//utfdata = new unsigned char[6];
	//utfdata = "������";
	//unicode_data = "......";

	//utf8_to_unicode();


	// echo
	AsioNetwork::Config config;
	config.nRecvOffset = 0;
	config.nStartId = 1;
	config.nMaxClient = 200;
	config.nMaxSendBuf = 5120;
	config.nMaxRecvBuf = 5120;
	config.nMaxClientPack = 512;
	config.nIoThreads = 1;
	config.nTimeOut = 0;
	config.nPreAcceptAmount = 10;
	config.fnCreateNetObject = createClientSocket;
	config.fnDestroyNetObject = destroyClientSocket;
	config.fnRealDelNetObject = realDeleteClientSocket;

	// AsioNetwork network;
	g_network.Init(config);
	g_network.StartListen(SHost, SPort);

	AsioNetwork::Config httpConfig;
	httpConfig.nRecvOffset = 0;
	httpConfig.nStartId = 1;
	httpConfig.nMaxClient = 200;
	httpConfig.nMaxSendBuf = 5120;
	httpConfig.nMaxRecvBuf = 5120;
	httpConfig.nMaxClientPack = 512;
	httpConfig.nIoThreads = 1;
	httpConfig.nTimeOut = 0;
	httpConfig.nPreAcceptAmount = 10;
	httpConfig.fnCreateNetObject = createHttpClient;
	httpConfig.fnDestroyNetObject = destroyHttpClient;
	httpConfig.fnRealDelNetObject = realDeleteHttpClient;
	g_httpServer.Init(httpConfig);
	g_httpServer.StartListen(SHHost, SHPort);

	printf("MsgServer IS Runing\n", "1.0.01");
 	InitDefMsgOut();
// 	OutputMsg(rmTip, _T("-------------------------------------------"));
// 	OutputMsg(rmTip, _T("�����������ɹ������İ汾����%s"), "1.0.01");



	while (1)
	{
		g_network.Update();
		g_httpServer.Update();
		g_DBQueue.Update();
		utils::timer_runonce();
		thread_sleep(1);
	}

	return 0;
}
