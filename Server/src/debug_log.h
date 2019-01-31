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
日志类.
支持按日期，小时，文件大小切换日志文件
未封装多线程同步机制. 
*/

const int DEFAULT_MAX_FILE_SIZE = (1024*1024*1024);     //最大文件SIZE为1G
const int DEFAULT_MAX_FILE_NO = 1;                      //默认最大文件编号
const int MAX_LOG_NAME_LEN = 1024;

//日志类型常量
enum LOG_TYPE
{
    LOG_TYPE_NORMAL = 0,	//普通日志, 不切换日志文件名
    LOG_TYPE_CYCLE = 1,		//按序号切换日志文件名
    LOG_TYPE_DAILY,			//按天切换日志文件名
    LOG_TYPE_HOURLY,		//按小时切换日志文件名
};

//日志级别常量
enum LOG_LEVEL
{
    LOG_TRACE = 1,			
    LOG_DEBUG = 2,
	LOG_INFO = 3,
    LOG_WARN = 4,
    LOG_ERROR = 5,
    LOG_FATAL = 6,
    LOG_NONE = 10000,     //用于设置不打印任何日志
};

extern char* LOG_LEVEL_STRING[];

//取时间的函数指针
//返回值同time();
//tv 可为NULL
typedef time_t (*TIME_FUNC)(struct timeval *tv); 

class DebugLog
{
protected:
    bool file_open_;		
    int log_level_;			//日志级别
    int log_type_;			//日志类型
    FILE  *file_;			//文件句柄
    int max_file_size_;		//最大文件尺寸
    int max_file_no_;      //最多文件个数, 仅当log_type_ == LOG_TYPE_NORMAL时有效

    int cur_file_size_;		//当前文件尺寸
    int cur_file_no_;		//当前文件编号


    time_t pre_time_;			//上一操作时间
    string path_;				//日志目录
    string name_prefix_;		//日志名前缀
    string filename_;			//日志名

	TIME_FUNC time_func_;		//取时间的函数

    static DebugLog* instance_;
public:
     //调试信息

    DebugLog();
    ~DebugLog();

    static DebugLog* instance();

	//创建instance, 并登记取时间函数
    static int init(TIME_FUNC time_func=NULL);
	//删除instance
    static void fini();
     
	//设置日志相关信息.
	//如果 path或name_prefix为空, 则输出到STD
	//max_file_size == 0 表示不检查文件尺寸
    int set_log(int log_level, int log_type=LOG_TYPE_NORMAL, char* path=NULL, char* name_prefix=NULL, 
                int max_file_size=DEFAULT_MAX_FILE_SIZE, int max_file_no=DEFAULT_MAX_FILE_NO);
    int set_log(int log_level, int log_type, string path, string name_prefix, 
                int max_file_size=DEFAULT_MAX_FILE_SIZE, int max_file_no=DEFAULT_MAX_FILE_NO);

	//设置日志级别
    void set_log_level(int level);

	//打印日志
    void log_p(int log_level, const char* FMT, ...);
    //打印日志, 日志中不输出时间
	void log_p_no_time(int log_level, const char* FMT, ...);
	
	//强制写文件
    void flush_file(void);

protected:

	//格式化时间串
	static char* GetPackTimeFormat(struct timeval &tv, char* szDateTime);
	static char* GetTimeFormat(struct timeval &tv, char* szDateTime);

	//强制换名, 如果目标文件已经存在, 则先删除.
    int force_rename(const char* src_pathname, const char* dest_pathname);
	//初始化文件编号, 从1到MAX遍历, 有空的则用之. 如果编号全部被使用, 则使用最OLD时间的文件的编号.
    int init_cur_file_no();
	//生成文件名
    int build_file_name(char* pathname=NULL, string* filename=NULL);
	//
    int open_file();
    
	//切换文件名, 按时间及文件尺寸检查
	int shift_file(struct timeval &tv);
	//
    void close_file();

	//检查日志级别, 打印日志前缀(时间,日志级别)
	int log_prepare(int log_level, struct timeval *tv);
    void log_i(int log_level, struct timeval *tv, const char* FMT, ...);    //内部LOG函数
    void log_i_v(int log_level, struct timeval *tv, const char* FMT, va_list ap);    //内部LOG函数
};


#define DEBUG_P DebugLog::instance()->log_p
#define DEBUG_P_NO_TIME DebugLog::instance()->log_p_no_time
#define DEBUG_OPEN DebugLog::instance()->set_log
#define DEBUG_INIT DebugLog::init
#define DEBUG_FINI DebugLog::fini

#endif //_DEBUG_LOG_H_

