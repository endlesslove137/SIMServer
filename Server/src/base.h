#ifndef _WEEDONG_CORE_OS_H_
#define _WEEDONG_CORE_OS_H_

#include <time.h>
#include <errno.h>

#ifndef WIN32
#define LINUX
#endif

#if defined LINUX

#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <semaphore.h>

#elif defined WIN32

#undef _WIN32_WINNT
#undef WINVER
#define _WIN32_WINNT 0x0501
#define WINVER 0x0501
#include <windows.h>
#include <winsock2.h>
#include <winbase.h>
#include <process.h>
#include <shlwapi.h>
#include <stdio.h>

#endif //#if defined LINUX

#if defined(WIN32) && defined(WD_BUILD_AS_DLL)

#if defined(WD_LIB)
#define WD_API __declspec(dllexport)
#else
#define WD_API __declspec(dllimport)
#endif

#else

#define WD_API 

#endif //#if defined(WIN32) && defined(WD_BUILD_AS_DLL)


typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef char int8;
typedef unsigned char uint8;

typedef short int16;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;

#ifdef LINUX
typedef long long int64;
typedef unsigned long long uint64;
#else
typedef __int64 int64;
typedef unsigned __int64 uint64;
#endif

// time
#if defined LINUX

#include <sys/time.h>
typedef time_t time64;

#elif defined WIN32

#include <time.h>
typedef __int64 time64;

#endif

// namespace wd
// {

//==================================================================
// ʱ���������
WD_API void gettime( time64 &pTime );   //��ȡ��ǰ time64 ʱ��
WD_API void localtime( struct tm &pTm, time64 &pTime );
WD_API struct tm localtime(void);

// thread
#if defined LINUX

typedef pthread_t         thread_id;
typedef pthread_t         thread_t;
typedef pthread_mutex_t   mutex_t;
typedef sem_t             semaphore_t;

#elif defined WIN32

typedef DWORD             thread_id;
typedef HANDLE            thread_t;
typedef CRITICAL_SECTION  mutex_t;
typedef HANDLE            semaphore_t;

#endif

//================================================================================================
// �̲߳�������

// �̻߳ص�����ԭ��
typedef unsigned int (*thread_proc_t)(void*);

/*************************************************
  Description:    // �����߳�
  Input:          // new_thread_handle �߳̾��
                  // start_routine �̻߳ص�����
                  // arg �̲߳���
  Output:         // 0 �ɹ� -1 ʧ��
  Return:         // 
  Others:         // 
*************************************************/
WD_API int      thread_create(thread_t* new_thread_handle, thread_proc_t start_routine, void* arg);

/*************************************************
  Description:    // �뿪�̲߳����ش����� �̻߳ص�������ʹ��
  Input:          // code ������
  Output:         // 
  Return:         // 
  Others:         // 
*************************************************/
WD_API void     thread_exit(unsigned long code);

/*************************************************
  Description:    // �ر��߳̾��
  Input:          // thread_handle �߳̾��
  Output:         // 
  Return:         // 
  Others:         // 
*************************************************/
WD_API void     thread_close_handle(thread_t* thread_handle);

/*************************************************
  Description:    // �ȴ��߳��˳�
  Input:          // thread_handle �߳̾��
  Output:         // 0 �ɹ� ���� ʧ��
  Return:         // 
  Others:         // 
*************************************************/
WD_API int      thread_wait_exit(thread_t* thread_handle);

/*************************************************
  Description:    // ȡ��ǰ�߳�id
  Input:          // 
  Output:         // thread_id �߳�ID
  Return:         // 
  Others:         // 
*************************************************/
WD_API thread_id    thread_get_current_id(void);

/*************************************************
  Description:    // sleep��ǰ�߳� 
  Input:          // milliseconds ����
  Output:         // 
  Return:         // 
  Others:         // 
*************************************************/
WD_API void     thread_sleep(uint32 milliseconds);

/*************************************************
  Description:    // ȡ�������� 
  Input:          // 
  Output:         // ����
  Return:         // 
  Others:         // 
*************************************************/
WD_API uint32   get_tick(void);
//================================================================================================


//================================================================================================
// �����������

/*************************************************
  Description:    // �������� 
  Input:          // mutex_ptr ���ָ��
  Output:         // 0 �ɹ� ����ʧ��
  Return:         // 
  Others:         // 
*************************************************/
WD_API int  mutex_create(mutex_t* mutex_ptr);

/*************************************************
  Description:    // �ͷŻ��� 
  Input:          // mutex_ptr ���ָ��
  Output:         // 0 �ɹ� ����ʧ��
  Return:         // 
  Others:         // 
*************************************************/
WD_API int  mutex_destroy(mutex_t* mutex_ptr);

/*************************************************
  Description:    // ������ 
  Input:          // mutex_ptr ���ָ��
  Output:         // 0 �ɹ� ����ʧ��
  Return:         // 
  Others:         // 
*************************************************/
WD_API int  mutex_lock(mutex_t* mutex_ptr);

/*************************************************
  Description:    // ���������� 
  Input:          // mutex_ptr ���ָ��
  Output:         // 0 �ɹ� ����ʧ��
  Return:         // 
  Others:         // ֻ�гɹ�ʱ����Ҫ����
*************************************************/
WD_API int  mutex_trylock(mutex_t* mutex_ptr);

/*************************************************
  Description:    // �������� 
  Input:          // mutex_ptr ���ָ��
  Output:         // 0 �ɹ� ����ʧ��
  Return:         // 
  Others:         // 
*************************************************/
WD_API int  mutex_unlock(mutex_t* mutex_ptr);
//================================================================================================


//================================================================================================
// ��������������

typedef struct _spin_lock_t* spin_lock_t;

/*************************************************
  Description:    // ���������� 
  Input:          // spin_lock ���ָ��
  Output:         // 
  Return:         // 
  Others:         // 
*************************************************/
WD_API void spin_lock_create(spin_lock_t* spin_lock);

/*************************************************
  Description:    // ���������� 
  Input:          // spin_lock ���ָ��
  Output:         // 
  Return:         // 
  Others:         // 
*************************************************/
WD_API void spin_lock_destroy(spin_lock_t* spin_lock);

/*************************************************
  Description:    // �������� 
  Input:          // spin_lock ���ָ��
  Output:         // 
  Return:         // 
  Others:         // 
*************************************************/
WD_API void spin_lock_lock(spin_lock_t* spin_lock);

/*************************************************
  Description:    // ������������ 
  Input:          // spin_lock ���ָ��
  Output:         // 0 �ɹ� ���� ʧ��
  Return:         // 
  Others:         // ֻ�гɹ�ʱ����Ҫ����
*************************************************/
WD_API int spin_lock_trylock(spin_lock_t* spin_lock);

/*************************************************
  Description:    // ���������� 
  Input:          // spin_lock ���ָ��
  Output:         // 0 �ɹ� ���� ʧ��
  Return:         // 
  Others:         // 
*************************************************/
WD_API int spin_lock_unlock(spin_lock_t* spin_lock);
//================================================================================================


//================================================================================================
// �ź�����������

/*************************************************
  Description:    // �����ź��� 
  Input:          // semaphore ���ָ��
                  // init_count �źŵĳ�ʼ����
                  // max_count �źŵ�������
  Output:         // 0 �ɹ� ���� ʧ��
  Return:         // 
  Others:         // 
*************************************************/
WD_API int semaphore_create(semaphore_t* semaphore, int init_count, int max_count);

/*************************************************
  Description:    // �ͷ�һ���ź��� 
  Input:          // semaphore ���ָ��
  Output:         // 0 �ɹ� ���� ʧ��
  Return:         // 
  Others:         // 
*************************************************/
WD_API int semaphore_put(semaphore_t* semaphore);

/*************************************************
  Description:    // ��ȡһ���ź� 
  Input:          // semaphore ���ָ��
                  // milli_secs �ȴ�������
  Output:         // 0 �ɹ� ���� ʧ��
  Return:         // 
  Others:         // 
*************************************************/
WD_API int semaphore_get(semaphore_t* semaphore, uint32 milli_secs);

/*************************************************
  Description:    // �ͷ��ź��� 
  Input:          // semaphore ���ָ��
  Output:         // 0 �ɹ� ���� ʧ��
  Return:         // 
  Others:         // 
*************************************************/
WD_API int semaphore_destroy(semaphore_t* semaphore);
//================================================================================================

class WD_API CThread
{
public:
    CThread();
    virtual ~CThread();

    void StartThread();
    thread_t* GetHandle();

    virtual void EndThread() = 0;
    virtual uint32 Run() = 0;

private:
    thread_t m_hThread;
};

class WD_API CMutex
{
public:
    CMutex();
    ~CMutex();

    void Lock();
    void UnLock();
    bool TryLock();

private:
    mutex_t m_Mutex;
};

class WD_API CSpinLock
{
public:
    CSpinLock();
    ~CSpinLock();

    void Lock();
    void UnLock();
    bool TryLock();

private:
    spin_lock_t m_SpinLock;
};

class WD_API CSemaphore
{
public:
    CSemaphore(int init_count, int max_count);
    ~CSemaphore();

    int Put();
    int Get(uint32 milli_secs);

private:
    semaphore_t m_Semaphore;
};

template<typename T>
class CGuard
{
public:
    CGuard(T* p):m_pLock(p)
    {
        m_pLock->Lock();
    }

    ~CGuard()
    {
        UnLock();
    }

    void UnLock()
    {
        if ( m_pLock != NULL )
            m_pLock->UnLock();
        m_pLock = NULL;
    }

private:
    T*  m_pLock;
};

// };

#endif
