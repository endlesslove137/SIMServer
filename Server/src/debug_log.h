#ifndef _DEBUG_LOG_H_
#define _DEBUG_LOG_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/timeb.h>
#include <time.h>

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>

#ifdef WIN32
#include <Winsock2.h>
#else
#include <sys/time.h>
#endif

using namespace std;


#ifdef WIN32
#define vsnprintf _vsnprintf
#define snprintf _snprintf

#define stat _stat

const char DIR_SEPERATOR ='\\';
#else

const char DIR_SEPERATOR ='/';
#endif


/*
��־��.
֧�ְ����ڣ�Сʱ���ļ���С�л���־�ļ�
δ��װ���߳�ͬ������. 
*/

const int DEFAULT_MAX_FILE_SIZE = (1024*1024*1024);     //����ļ�SIZEΪ1G
const int DEFAULT_MAX_FILE_NO = 1;                      //Ĭ������ļ����
const int MAX_LOG_NAME_LEN = 1024;

//��־���ͳ���
enum LOG_TYPE
{
    LOG_TYPE_NORMAL = 0,	//��ͨ��־, ���л���־�ļ���
    LOG_TYPE_CYCLE = 1,		//������л���־�ļ���
    LOG_TYPE_DAILY,			//�����л���־�ļ���
    LOG_TYPE_HOURLY,		//��Сʱ�л���־�ļ���
};

//��־������
enum LOG_LEVEL
{
    LOG_TRACE = 1,			
    LOG_DEBUG = 2,
	LOG_INFO = 3,
    LOG_WARN = 4,
    LOG_ERROR = 5,
    LOG_FATAL = 6,
    LOG_NONE = 10000,     //�������ò���ӡ�κ���־
};

extern char* LOG_LEVEL_STRING[];

//ȡʱ��ĺ���ָ��
//����ֵͬtime();
//tv ��ΪNULL
typedef time_t (*TIME_FUNC)(struct timeval *tv); 

class DebugLog
{
protected:
    bool file_open_;		
    int log_level_;			//��־����
    int log_type_;			//��־����
    FILE  *file_;			//�ļ����
    int max_file_size_;		//����ļ��ߴ�
    int max_file_no_;      //����ļ�����, ����log_type_ == LOG_TYPE_NORMALʱ��Ч

    int cur_file_size_;		//��ǰ�ļ��ߴ�
    int cur_file_no_;		//��ǰ�ļ����


    time_t pre_time_;			//��һ����ʱ��
    string path_;				//��־Ŀ¼
    string name_prefix_;		//��־��ǰ׺
    string filename_;			//��־��

	TIME_FUNC time_func_;		//ȡʱ��ĺ���

    static DebugLog* instance_;
public:
     //������Ϣ

    DebugLog();
    ~DebugLog();

    static DebugLog* instance();

	//����instance, ���Ǽ�ȡʱ�亯��
    static int init(TIME_FUNC time_func=NULL);
	//ɾ��instance
    static void fini();
     
	//������־�����Ϣ.
	//��� path��name_prefixΪ��, �������STD
	//max_file_size == 0 ��ʾ������ļ��ߴ�
    int set_log(int log_level, int log_type=LOG_TYPE_NORMAL, char* path=NULL, char* name_prefix=NULL, 
                int max_file_size=DEFAULT_MAX_FILE_SIZE, int max_file_no=DEFAULT_MAX_FILE_NO);
    int set_log(int log_level, int log_type, string path, string name_prefix, 
                int max_file_size=DEFAULT_MAX_FILE_SIZE, int max_file_no=DEFAULT_MAX_FILE_NO);

	//������־����
    void set_log_level(int level);

	//��ӡ��־
    void log_p(int log_level, const char* FMT, ...);
    //��ӡ��־, ��־�в����ʱ��
	void log_p_no_time(int log_level, const char* FMT, ...);
	
	//ǿ��д�ļ�
    void flush_file(void);

protected:

	//��ʽ��ʱ�䴮
	static char* GetPackTimeFormat(struct timeval &tv, char* szDateTime);
	static char* GetTimeFormat(struct timeval &tv, char* szDateTime);

	//ǿ�ƻ���, ���Ŀ���ļ��Ѿ�����, ����ɾ��.
    int force_rename(const char* src_pathname, const char* dest_pathname);
	//��ʼ���ļ����, ��1��MAX����, �пյ�����֮. ������ȫ����ʹ��, ��ʹ����OLDʱ����ļ��ı��.
    int init_cur_file_no();
	//�����ļ���
    int build_file_name(char* pathname=NULL, string* filename=NULL);
	//
    int open_file();
    
	//�л��ļ���, ��ʱ�估�ļ��ߴ���
	int shift_file(struct timeval &tv);
	//
    void close_file();

	//�����־����, ��ӡ��־ǰ׺(ʱ��,��־����)
	int log_prepare(int log_level, struct timeval *tv);
    void log_i(int log_level, struct timeval *tv, const char* FMT, ...);    //�ڲ�LOG����
    void log_i_v(int log_level, struct timeval *tv, const char* FMT, va_list ap);    //�ڲ�LOG����
};


#define DEBUG_P DebugLog::instance()->log_p
#define DEBUG_P_NO_TIME DebugLog::instance()->log_p_no_time
#define DEBUG_OPEN DebugLog::instance()->set_log
#define DEBUG_INIT DebugLog::init
#define DEBUG_FINI DebugLog::fini

#endif //_DEBUG_LOG_H_

