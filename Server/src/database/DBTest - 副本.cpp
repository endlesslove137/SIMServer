#include "base.h"
#include "Database.h"
#include "log_wrap.h"

DBRequestQueue g_DBQueue;

class CDBGenerExec: public DBRequest
{
public:
    void OnRequest(Database& db)
    {
        RecordsetPtr result = db.Query("select 12314;");
        while (result.get() && result->MoveNext())
        {
            ((*result) >> nResult);
        }


    }

    void OnFinish()
    {
        printf("thread db result:%d\n", nResult);
    }

    int nResult;
};


void DBTest()
{
    // 基本
    MYSQL_PARAM log_db_param;
    log_db_param.nPort = 3306;
    log_db_param.strDefaultDB = "mysql";
    log_db_param.strHost = "127.0.0.1";
    log_db_param.strLogin = "root";
    log_db_param.strPasswd = "123456";

    Database db;
    db.SetDBConfig(log_db_param);

    if (!db.Logon())
    {
        WRITE_ERR_LOG("DB connect faild");
        return;
    }

    RecordsetPtr result = db.Query("select 12314;");
    while (result.get() && result->MoveNext())
    {
        uint32 i = 0;
        ((*result) >> i);
        printf("db result:%d\n", i);
    }

    // 多线程
    g_DBQueue.Init(3, log_db_param);

    boost::shared_ptr<CDBGenerExec> request(new CDBGenerExec);
    g_DBQueue.AddQueueMsg(request);
}

