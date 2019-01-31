#ifndef _LOG_WRAP_H_
#define _LOG_WRAP_H_

#include "debug_log.h"
#include "log_manager.h"
#include "assert.h"

#ifndef WIN32

// 需要使用ISO C99标准的一些功能(macro可变参数)。C++不支持,C99标准已经支持
/* We use __extension__ in some places to suppress -pedantic warnings
about GCC extensions.  This feature didn't work properly before
gcc 2.8.  */

#if GCC_VERSION < 2008
#define __extension__
#endif /* GCC_VERSION < 2008 */
#else /* WIN32 */

#define __extension__

// while(0)在警告最高等级打开后会报4127警告
// 使用while(0)可以保证调用者把宏当着一行普通的语句一样处理，减少异常发生的可能
#pragma warning(disable: 4127)

#endif /* WIN32 */

inline bool IsTraceUin(unsigned int uin)
{
	if( g_LogCfg.AllTraceFlag != 0 )
	{
		return true;
	}
	for( int i = 0; i < g_LogCfg.TraceNum; i++ )
	{
		if( g_LogCfg.Traceuins[i] == uin )
		{
			return true;
		}
	}
	return false;
}

#ifndef LOG_FILE_LINE
__extension__
#define WRITE_DBG_LOG(...) \
	do \
{\
	if( g_LogCfg.DebugLogFlag != 0 ) \
{   \
	DebugLog::instance()->log_p(LOG_DEBUG, __VA_ARGS__); \
}\
} while (0)

__extension__
#define WRITE_WARN_LOG(...) \
	do \
{\
	if( g_LogCfg.WarnLogFlag != 0 ) \
{   \
	DebugLog::instance()->log_p(LOG_WARN, __VA_ARGS__); \
}\
} while (0)

__extension__
#define WRITE_ERR_LOG(...) \
	do \
{ \
	DebugLog::instance()->log_p(LOG_ERROR, __VA_ARGS__);  \
} while (0)

// 致命日志打印
__extension__
#define WRITE_FATAL_LOG(...) \
	do \
{ \
	DebugLog::instance()->log_p(LOG_FATAL, __VA_ARGS__);  \
} while (0)

__extension__
#define WRITE_TRACE_LOG(uin, format, ...) \
	do \
{ \
	char tmpFormat__[10*1024];\
	snprintf(tmpFormat__, sizeof(tmpFormat__), "[%u]%s", uin, format);\
	if( g_LogCfg.DebugLogFlag != 0 ) \
{   \
	DebugLog::instance()->log_p(LOG_DEBUG, tmpFormat__, __VA_ARGS__); \
}\
	if( IsTraceUin(uin) )\
{ \
	DebugLog::instance()->log_p(LOG_TRACE, tmpFormat__, __VA_ARGS__);\
}\
} while (0)

__extension__
#define WRITE_INFO_LOG(...) \
	do \
{ \
	DebugLog::instance()->log_p(LOG_INFO, __VA_ARGS__);  \
} while (0)

__extension__
#define WRITE_LOG_FILE(szFile, format, ...) \
	do \
{ \
	char tmpFormat__[10*1024];\
	snprintf(tmpFormat__, sizeof(tmpFormat__), "[%s]%s", szFile, format);\
	DebugLog::instance()->log_p(LOG_DEBUG, tmpFormat__, ##__VA_ARGS__); \
} while (0)

#else /* LOG_FILE_LINE */
// 增加
#ifndef WIN32
#define LOG_PREFIX __FILE__, __LINE__, __func__
#else /* WIN32 */
#define LOG_PREFIX __FILE__, __LINE__, __FUNCTION__
#endif /* WIN32 */


__extension__
#define WRITE_DBG_LOG(format, ...) \
	do \
{\
	if( g_LogCfg.DebugLogFlag != 0 ) \
{ \
	char tmpFormat__[10*1024];\
	snprintf(tmpFormat__, sizeof(tmpFormat__), "%s:%d:%s: %s", LOG_PREFIX, format);\
	DebugLog::instance()->log_p(LOG_DEBUG, tmpFormat__, ##__VA_ARGS__); \
}\
} while (0)

__extension__
#define WRITE_WARN_LOG(format, ...) \
	do \
{\
	if( g_LogCfg.WarnLogFlag != 0 ) \
{   \
	char tmpFormat__[10*1024];\
	snprintf(tmpFormat__, sizeof(tmpFormat__), "%s:%d:%s: %s", LOG_PREFIX, format);\
	DebugLog::instance()->log_p(LOG_WARN, tmpFormat__, ##__VA_ARGS__); \
}\
} while (0)

__extension__
#define WRITE_ERR_LOG(format, ...) \
	do \
{ \
	char tmpFormat__[10*1024];\
	snprintf(tmpFormat__, sizeof(tmpFormat__), "%s:%d:%s: %s", LOG_PREFIX, format);\
	DebugLog::instance()->log_p(LOG_ERROR, tmpFormat__, ##__VA_ARGS__);  \
} while (0)

__extension__
#define WRITE_FATAL_LOG(format, ...) \
	do \
{ \
	char tmpFormat__[10*1024];\
	snprintf(tmpFormat__, sizeof(tmpFormat__), "%s:%d:%s: %s", LOG_PREFIX, format);\
	DebugLog::instance()->log_p(LOG_FATAL, tmpFormat__, ##__VA_ARGS__);  \
} while (0)

__extension__
#define WRITE_TRACE_LOG(uin, format, ...) \
	do \
{ \
	char tmpFormat__[10*1024];\
	snprintf(tmpFormat__, sizeof(tmpFormat__), "[%u]%s:%d:%s: %s", uin, LOG_PREFIX, format);\
	if( g_LogCfg.DebugLogFlag != 0 ) \
{   \
	DebugLog::instance()->log_p(LOG_DEBUG, tmpFormat__, ##__VA_ARGS__); \
}\
	if( IsTraceUin(uin) )\
{ \
	DebugLog::instance()->log_p(LOG_TRACE, tmpFormat__, ##__VA_ARGS__);\
}\
} while (0)

__extension__
#define WRITE_INFO_LOG(format, ...) \
	do \
{ \
	char tmpFormat__[10*1024];\
	snprintf(tmpFormat__, sizeof(tmpFormat__), "%s:%d:%s: %s", LOG_PREFIX, format);\
	DebugLog::instance()->log_p(LOG_INFO, tmpFormat__, ##__VA_ARGS__);  \
} while (0)

__extension__
#define WRITE_LOG_FILE(szFile, format, ...) \
	do \
{ \
	char tmpFormat__[10*1024];\
	snprintf(tmpFormat__, sizeof(tmpFormat__), "[%s]%s:%d:%s: %s", szFile, LOG_PREFIX, format);\
	DebugLog::instance()->log_p(LOG_DEBUG, tmpFormat__, ##__VA_ARGS__);  \
} while (0)

#endif /* LOG_FILE_LINE */

//#define CHECK_USE_ASSERT
#if defined(_DEBUG) && defined(CHECK_USE_ASSERT)
#undef		ASSERT
#define		ASSERT(x)	{ if(!(x)) assert(!("ASSERT: " #x)); }
#define		CHECK(x)	{ if(!(x)) { assert(!("CHECK: " #x)); return;} }
#define		CHECKF(x)	{ if(!(x)) { assert(!("CHECKF: " #x)); return 0;} }
#define		IF_NOT(x)	if( ((x)) ? 0 : ( assert(!("IF_NOT: " #x)),1 ) )
#define		IF_OK(x)	if( ((x)) ? 1 : ( assert(!("IF_OK: " #x)),0 ) )
#else
#undef		ASSERT
#define		ASSERT(x)	{ if(!(x)) DebugLog::instance()->log_p(LOG_WARN, "ASSERT( "#x" ) IN %s, %d", __FILE__, __LINE__); }
#define		CHECK(x)	{ if(!(x)) { DebugLog::instance()->log_p(LOG_WARN, "CHECK( "#x" ) IN %s, %d", __FILE__, __LINE__); return;} }
#define		CHECKF(x)	{ if(!(x)) { DebugLog::instance()->log_p(LOG_WARN, "CHECKF( "#x" ) IN %s, %d", __FILE__, __LINE__); return 0;} }
#define		IF_NOT(x)	if( ((x)) ? 0 : ( DebugLog::instance()->log_p(LOG_WARN, "IF_NOT( "#x" ) IN %s, %d", __FILE__, __LINE__),1 ) )
#define		IF_OK(x)	if( ((x)) ? 1 : ( DebugLog::instance()->log_p(LOG_WARN, "IF_OK( "#x" ) IN %s, %d", __FILE__, __LINE__),0 ) )

#endif

#endif //_LOG_WRAP_H_
