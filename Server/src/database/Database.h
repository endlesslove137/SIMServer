#pragma once
#ifndef __DATABASE_H__
#define __DATABASE_H__
#include <thread>
#include <memory>
#include <mutex>
#include "base.h"
#include <string>
#include <mysql.h>
#include <list>
#include <vector>



typedef struct _MYSQL_PARAM
{
	unsigned int nPort;
	std::string strLogin;
	std::string strPasswd;
	std::string strDefaultDB;
	std::string strHost;
}MYSQL_PARAM;

class Recordset
{
public:
	Recordset(MYSQL_RES* pMysqlRes);
	virtual ~Recordset();

	bool MoveNext();
	bool operator >> ( unsigned long long& nValue );
	bool operator >> ( unsigned int& nValue );
	bool operator >> ( unsigned short& nValue );
	bool operator >> ( int& nValue );
	bool operator >> ( short& nValue );
	bool operator >> ( unsigned char& nValue );
	bool operator >> ( char& nValue );
	bool operator >> ( std::string& strValue );
	bool NextField();

protected:
	unsigned int       m_nIndex;
	unsigned int       m_NumFields;
	MYSQL_ROW  m_MysqlRow;
	MYSQL_RES* m_pMysqlRes;
};

typedef std::shared_ptr<Recordset> RecordsetPtr;

class Database
{
public:
	Database():m_pMysql( NULL ){};
	virtual ~Database(){ Logoff(); };

    void SetDBConfig( MYSQL_PARAM dbParam );
    bool GetConnection( );

	bool Logon();
	void Logoff();
	bool IsConnected();
	my_ulonglong GetAffectedRows();
	RecordsetPtr Query( const char* szSQL, int nCommit = 1 );
    my_ulonglong GetInsertId();

private:
    MYSQL_PARAM m_DBParam;
	MYSQL* m_pMysql;
	my_ulonglong numAffectedRows;
};

class DBRequest
{
public:
	virtual ~DBRequest(){};
	virtual void OnRequest( Database& db ) = 0;
    virtual void OnFinish() = 0;
};

class DBRequestQueue
{
public:
    DBRequestQueue(){};
	virtual ~DBRequestQueue(){};

    void Init(int nThread, MYSQL_PARAM dbParam);

	void AddQueueMsg(std::shared_ptr<DBRequest> request);
    int Update();

private:
    std::shared_ptr<DBRequest> GetRequest();
    void FinishRequest(std::shared_ptr<DBRequest> request);
    static void DBWorkThread(DBRequestQueue* pDBQueue, MYSQL_PARAM dbParam);

private:
    typedef std::list<std::shared_ptr<DBRequest> > DBRequestList;
    std::mutex m_WorkMutex;
    DBRequestList m_TempQueue;
    DBRequestList m_WorkQueue;
    std::mutex m_FinishMutex;
    DBRequestList m_FinishQueue;

    std::vector< std::shared_ptr<std::thread> > m_WorkThread;
};

#endif //define __DATABASE_H__
