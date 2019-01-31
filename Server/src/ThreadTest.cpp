#include "base.h"

class MyThread: public CThread
{
public:
    virtual uint32 Run()
    {
        time_t rawtime;

        time ( &rawtime );
        printf ( "MyThread:%d :%s\n", type, ctime(&rawtime) );
		
        for (int i=0; i<10; i++)
        {
            printf("%d %d\n", type, i);
        }

        time ( &rawtime );
        printf ( "EndThread:%d :%s\n", type, ctime(&rawtime) );
        return 0;
    }

    virtual void EndThread()
    {

    }

    int type;
};


void ThreadTest()
{
    MyThread t1;
    t1.type = 1;
    t1.StartThread();
    MyThread t2;
    t2.type = 2;
    t2.StartThread();

    thread_wait_exit(t1.GetHandle());
    thread_wait_exit(t2.GetHandle());
}




void TimerTest()
{

}