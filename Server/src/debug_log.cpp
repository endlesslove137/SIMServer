#include "debug_log.h"
#include "log_manager.h"
#include "base.h"

tagLogCfg g_LogCfg;

char* LOG_LEVEL_STRING[] = {
	"",
	"TRACE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL",
};

#	ifdef WIN32
/*
WINDOWS下模拟gettimeofday函数
两种方式都有效, 不过在PC机上的精度有限(15-16 ms)
在LINUX环境下测试, 精度可以到 1ms
*/
int gettimeofday(struct timeval *tv, struct timezone *)
{

	struct _timeb stTime;
	assert(tv != NULL);
	_ftime( &stTime );
	tv->tv_sec = (long)stTime.time;
	tv->tv_usec = 1000 * stTime.millitm;

	/*
	SYSTEMTIME tsys;
	FILETIME   file_time;
	::GetSystemTime (&tsys);
	::SystemTimeToFileTime (&tsys, &file_time);

	ULARGE_INTEGER _100ns;
	_100ns.LowPart = file_time.dwLowDateTime;
	_100ns.HighPart = file_time.dwHighDateTime;

	_100ns.QuadPart -= 0x19db1ded53e8000;

	// Convert 100ns units to seconds;
	tv->tv_sec = (long) (_100ns.QuadPart / (10000 * 1000));
	// Convert remainder to microseconds;
	tv->tv_usec = (long) ((_100ns.QuadPart % (10000 * 1000)) / 10);
	*/
	return 0;
}   
#	endif /* WIN32 */	

time_t DefaultTimeFunc(struct timeval *tv)
{
	struct timeval stv;
	gettimeofday(&stv, NULL);
	if (tv != NULL)
	{
		*tv = stv;
	}
	return stv.tv_sec;
}

DebugLog* DebugLog::instance_ = NULL;

static CMutex s_LogLock;

DebugLog::DebugLog()
{
    CGuard<CMutex> lock(&s_LogLock);

	memset(&g_LogCfg, 1, sizeof(g_LogCfg));

	time_func_ = DefaultTimeFunc;

	log_level_ = LOG_TRACE;
    log_type_ = LOG_TYPE_NORMAL;

    file_open_ = false;
    file_ = stdout;

    pre_time_ = time_func_(NULL);

    max_file_size_ = DEFAULT_MAX_FILE_SIZE;
    max_file_no_ = DEFAULT_MAX_FILE_NO;

    cur_file_size_ = 0;
    cur_file_no_ = 1;

}

DebugLog::~DebugLog()
{
    CGuard<CMutex> lock(&s_LogLock);

    close_file();
}

void DebugLog::close_file()
{
    CGuard<CMutex> lock(&s_LogLock);

    if (file_open_)
    {
        fclose(file_);
        file_ = stdout;
        file_open_ = false;
    }
}

void DebugLog::flush_file(void)
{
    CGuard<CMutex> lock(&s_LogLock);

    if (file_open_)
    {
        fflush(file_);
    }
}

DebugLog* DebugLog::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new  DebugLog;
    }
    return instance_;
}

int DebugLog::init(TIME_FUNC time_func)
{
    CGuard<CMutex> lock(&s_LogLock);

    if (instance() == NULL)
    {
        return -1;
    }
	if (time_func != NULL)
	{
		instance()->time_func_ = time_func;
	}
    return 0;
}

void DebugLog::fini()
{
    CGuard<CMutex> lock(&s_LogLock);

    if (instance_ != NULL)
    {
        delete instance_;
        instance_ = NULL;
    }
}

int DebugLog::set_log(int log_level, int log_type, char* path, 
                    char* name_prefix, int max_file_size, int max_file_no)
{
    CGuard<CMutex> lock(&s_LogLock);

    log_level_ = log_level;
    log_type_ = log_type;

    if (path == NULL || name_prefix == NULL)
    {
        //表示使用stdout
        close_file();
        name_prefix_ = "";
        path_ = "";
    }
    else
    {
        name_prefix_ = name_prefix;
        path_ = path;
        if (path_[path_.length()-1] == DIR_SEPERATOR)
        {
            path_[path_.length()-1] = 0;
        }
    }

    max_file_size_ = max_file_size;
    if (max_file_size_ > DEFAULT_MAX_FILE_SIZE) 
	{ 
		max_file_size_ = DEFAULT_MAX_FILE_SIZE; 
	};

    max_file_no_ = max_file_no;

    init_cur_file_no();
    
    return open_file();
}

int DebugLog::init_cur_file_no()
{
    CGuard<CMutex> lock(&s_LogLock);

    cur_file_no_ = 1;
    if (name_prefix_.length() == 0) 
	{ 
		return 0; /*直接使用stdout*/
	}
    if (log_type_ == LOG_TYPE_NORMAL) 
	{ 
		return 0; /*普通模式不需要编号*/
	}

    //查找当前最大编号
    char base_file[MAX_LOG_NAME_LEN];
    build_file_name(base_file);

    int file_no = 1;
    time_t min_modify_time = 0;

	struct timeval tv;
    time_t cur_time = time_func_(&tv);
    
    for (int i = 1; i <= max_file_no_; ++i)
    {
		//循环查找未使用的编号
        char dest_file[MAX_LOG_NAME_LEN];
        snprintf(dest_file, MAX_LOG_NAME_LEN, "%s.%d", base_file, i);
        dest_file[MAX_LOG_NAME_LEN -1] = 0;

        DebugLog::instance()->log_i(LOG_DEBUG, &tv, "stat file:%s \n", dest_file);

        struct stat sb;
        if (stat(dest_file, &sb) != 0)
        {
            //文件不存在, 使用该编号
            file_no = i;
            DebugLog::instance()->log_i(LOG_DEBUG, &tv, "dest_file not exist:%s \n", dest_file);
            break;  
        }
        if (LOG_TYPE_CYCLE == log_type_)
        {
            if ( i >= max_file_no_)
            {
                break;
            }
            //循环日志, 需要按最后修改时候进行排序
            if (min_modify_time == 0 || sb.st_mtime < min_modify_time)
            {
                file_no = i;
                min_modify_time = sb.st_mtime;
            }
        }
    }

    cur_file_no_ = file_no;
    DebugLog::instance()->log_i(LOG_INFO, &tv, "cur_file_no:%d \n", cur_file_no_);

    return 0;
}


int DebugLog::set_log(int log_level, int log_type, string path, string name_prefix, 
            int max_file_size, int max_file_no)
{
    CGuard<CMutex> lock(&s_LogLock);

    if (path.length() <= 0 || name_prefix.length() <=0)
    {
        return set_log(log_level, log_type, NULL, NULL, max_file_size, max_file_no);
    }
    return set_log(log_level, log_type, (char*)path.c_str(), (char*)name_prefix.c_str(), max_file_size, max_file_no);
}

int DebugLog::build_file_name(char* pathname, string* filename)
{
    CGuard<CMutex> lock(&s_LogLock);

    char time_str[50];
	struct timeval tv;
	tv.tv_sec = (long)pre_time_;
	tv.tv_usec = 0;
    GetPackTimeFormat(tv, time_str);
    switch (log_type_)
    {    
    case LOG_TYPE_DAILY:
        {
            time_str[8] = '.'; //只需要到天数
            time_str[9] = 0;
            break;
        }
    case LOG_TYPE_HOURLY:
        {
            time_str[10] = '.'; //只需要到小时
            time_str[11] = 0;
            break;
        }
    default:
        {
            time_str[0] = 0; //不需要记录时间
            break;
        }
    }
    
    string file_name = name_prefix_ + "." + time_str+ "log";
    if (filename != NULL)
    {
        *filename = file_name;
    }
    if (pathname != NULL)
    {
        snprintf(pathname, MAX_LOG_NAME_LEN, "%s%c%s", 
                    path_.c_str(), DIR_SEPERATOR, file_name.c_str());
        pathname[MAX_LOG_NAME_LEN-1]=0;
    }
    return 0;
}

int DebugLog::open_file()
{
    CGuard<CMutex> lock(&s_LogLock);

    if (file_open_) 
	{ 
		return 0; 
	}
    if (name_prefix_.length() == 0) 
	{
		return 0; /*直接使用stdout*/
	}

    char pathname[MAX_LOG_NAME_LEN];
    build_file_name(pathname, &filename_);

    if ((file_ = fopen(pathname, "a+")) == NULL)
    {
		//文件打开失败, 则使用标准输出
        file_ = stdout;
        file_open_ = false;
        name_prefix_ = "";
		struct timeval tv;
        time_func_(&tv);
        DebugLog::instance()->log_i(LOG_FATAL, &tv, "log fopen fail:%s\n", pathname);
        return -1;
    }

    file_open_ = true;
    
    struct stat sb;
    if (stat(pathname, &sb) < 0)
    {
        cur_file_size_ = 0; 
    }
    else
    {
        cur_file_size_ = (int) sb.st_size;
    }
    
    return 0;
}

#ifdef WIN32
struct tm* localtime_r(time_t* t, struct tm* stm)
{
    memcpy(stm, localtime(t), sizeof(struct tm));
    return stm;
}
#endif

int DebugLog::force_rename(const char* src_pathname, const char* dest_pathname)
{
    CGuard<CMutex> lock(&s_LogLock);

    remove(dest_pathname);
    if (rename(src_pathname, dest_pathname) != 0) { return -1; }
    return 0;
}

int DebugLog::shift_file(struct timeval &tv)
{
    CGuard<CMutex> lock(&s_LogLock);

    if (name_prefix_.length() == 0)
    {
        //未定义文件名前缀, 表示使用stdout, 不需要进行文件切换
        return 0;
    }
    if (log_type_ == LOG_TYPE_NORMAL)
    {
        //普通日志, 不需要滚动
        return 0;
    }

    int need_shift = 0;	/* 0: 不切换; 1: 因时间切换; 2: 因文件尺寸切换*/
    
	time_t cur_time = tv.tv_sec;
    struct tm cur_tm ;
    struct tm pre_tm ;
    localtime_r((time_t*)&cur_time, &cur_tm);
    localtime_r((time_t*)&pre_time_, &pre_tm);

    int new_file_no = 0;
    
    //先进行时间检查
    switch (log_type_)
    {    
    case LOG_TYPE_DAILY:
        {
            if ((cur_tm.tm_year != pre_tm.tm_year) || (cur_tm.tm_yday != pre_tm.tm_yday))
            {
                //天数不等, 说明日期已经切换
                need_shift = 1;
                new_file_no = 1;
            }
            break;
        }
    case LOG_TYPE_HOURLY:
        {
            if ((cur_tm.tm_year != pre_tm.tm_year) || (cur_tm.tm_yday != pre_tm.tm_yday) || (cur_tm.tm_hour != pre_tm.tm_hour))
            {
                //天数或小时不等, 说明小时已经切换
                need_shift = 1;
                new_file_no = 1;
            }
            break;
        }
    default:
        break;
        
    }

    //再进行文件SIZE检查
    if (need_shift == 0)
    {
		if (max_file_size_ > 0)
		{
			if (cur_file_size_ >= max_file_size_)
			{
				need_shift = 2;
				new_file_no = cur_file_no_ + 1;
				if ((log_type_ == LOG_TYPE_CYCLE) && (new_file_no > max_file_no_))
				{
					//滚动日志, 需要检查最大文件编号
					new_file_no = 1;
				}
			}
		}
    }

    if (need_shift <= 0)
    {
        return 0; //不需要进行文件切换
    }

    close_file(); //先关闭当前文件

	if (need_shift == 2)
	{
		//按尺寸切换的, 需要move原文件
		char src_file[MAX_LOG_NAME_LEN];
		snprintf(src_file, MAX_LOG_NAME_LEN, "%s%c%s", 
			path_.c_str(), DIR_SEPERATOR, filename_.c_str());
		src_file[MAX_LOG_NAME_LEN -1] = 0;

		char dest_file[MAX_LOG_NAME_LEN];
		snprintf(dest_file, MAX_LOG_NAME_LEN, "%s.%d", src_file, cur_file_no_);
		dest_file[MAX_LOG_NAME_LEN -1] = 0;

		if (force_rename(src_file, dest_file) < 0 )
		{
			DebugLog::instance()->log_i(LOG_FATAL, &tv, "log rename fail (%d): %s ==> %s \n", errno, filename_.c_str(), dest_file);
			return -1;
		}
	}

    //打开新文件
    pre_time_ = cur_time;
    cur_file_no_ = new_file_no;
    return open_file();
}



void DebugLog::log_p(int log_level, const char* FMT, ...)
{
    CGuard<CMutex> lock(&s_LogLock);

    if (log_level < log_level_)   
	{  
		return ;  
	}
	struct timeval tv;
    time_func_(&tv);
    shift_file(tv);

    va_list ap;
    va_start(ap, FMT);
    log_i_v(log_level, &tv, FMT, ap);
    va_end(ap);
	fflush(file_);
}
void DebugLog::log_p_no_time(int log_level, const char* FMT, ...)
{
    CGuard<CMutex> lock(&s_LogLock);

    if (log_level < log_level_)   
	{  
		return ;  
	}

	struct timeval tv;
    time_func_(&tv);
    shift_file(tv);

    va_list ap;
    va_start(ap, FMT);
    log_i_v(log_level, NULL, FMT, ap);
    va_end(ap);
	fflush(file_);
}

char* DebugLog::GetPackTimeFormat(struct timeval &tv, char* szDateTime)
{
    CGuard<CMutex> lock(&s_LogLock);

	time_t iTime = tv.tv_sec;
    struct tm stTm ;
    localtime_r(&iTime, &stTm);
    
    sprintf(szDateTime, "%04d%02d%02d%02d%02d%02d%03d",
        stTm.tm_year+1900, stTm.tm_mon+1, stTm.tm_mday,
        stTm.tm_hour, stTm.tm_min, stTm.tm_sec, (tv.tv_usec/1000));
    
    return szDateTime;
};

char* DebugLog::GetTimeFormat(struct timeval &tv, char* szDateTime)
{
    CGuard<CMutex> lock(&s_LogLock);

	time_t iTime = tv.tv_sec;
    struct tm stTm ;
    localtime_r(&iTime, &stTm);
    
    sprintf(szDateTime, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
        stTm.tm_year+1900, stTm.tm_mon+1, stTm.tm_mday,
        stTm.tm_hour, stTm.tm_min, stTm.tm_sec, (tv.tv_usec/1000));
    
    return szDateTime;
};


void DebugLog::log_i(int log_level, struct timeval *tv, const char * FMT,...)
{
    CGuard<CMutex> lock(&s_LogLock);

	if (log_prepare(log_level, tv) != 0)
	{
		return;
	}

	int ret = 0;

	//再打印日志
    va_list ap;
    va_start(ap, FMT);
	//自动加上换行符
	char tmpFormat[10*1024];
	snprintf(tmpFormat, sizeof(tmpFormat)-1, "%s\n", FMT);
	tmpFormat[sizeof(tmpFormat)-1] = 0;
    ret = vfprintf(file_, tmpFormat, ap);
    va_end(ap);
    if (ret > 0)
    {
        cur_file_size_ += ret;
    }    
};

void DebugLog::log_i_v(int log_level, struct timeval *tv, const char * FMT, va_list ap)
{
    CGuard<CMutex> lock(&s_LogLock);

	if (log_prepare(log_level, tv) != 0)
	{
		return;
	}

	int ret = 0;

	//自动加上换行符
	char tmpFormat[10*1024];
	snprintf(tmpFormat, sizeof(tmpFormat)-1, "%s\n", FMT);
	tmpFormat[sizeof(tmpFormat)-1] = 0;

	//再打印日志
    ret = vfprintf(file_, tmpFormat, ap);
    if (ret > 0)
    {
        cur_file_size_ += ret;
    }    
};

void DebugLog::set_log_level(int level)
{
    CGuard<CMutex> lock(&s_LogLock);

    assert((level >= LOG_TRACE) && (level <= LOG_NONE));
    log_level_ = level;
}

int DebugLog::log_prepare(int log_level, struct timeval *tv)
{
    CGuard<CMutex> lock(&s_LogLock);

    assert((log_level >= LOG_TRACE) && (log_level < LOG_NONE));
    if (log_level < log_level_)   
    {  
        //级别不够, 不需要打印
        return -1; 
    }

    //先打印前缀
    int ret = 0;
    if (tv != NULL)
    {
        char time_str[50]="";
        GetTimeFormat(*tv, time_str );
        ret = fprintf(file_, "[%s][%s]",time_str, LOG_LEVEL_STRING[log_level]);
    }
    else
    {
        ret = fprintf(file_, "[%s]", LOG_LEVEL_STRING[log_level]);
    }
    if (ret > 0)
    {
        cur_file_size_ += ret;
    }  
    return 0;
};
