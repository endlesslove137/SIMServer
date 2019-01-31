#include "Database.h"
#include <string>
#include <fstream>
#include <time.h>
#include <boost/lexical_cast.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <functional>
#include <chrono>
Recordset::Recordset( MYSQL_RES* pMysqlRes ) : m_pMysqlRes( pMysqlRes )
{
	m_NumFields = mysql_num_fields( m_pMysqlRes);
	m_MysqlRow = NULL;
}

Recordset::~Recordset()
{
	mysql_free_result( m_pMysqlRes );
	m_pMysqlRes = NULL;
}

/////////////////////////////////////////////////////////////////////////////

void Database::SetDBConfig(MYSQL_PARAM dbParam)
{
    m_DBParam = dbParam;
}

bool Database::GetConnection( )
{
	return ( IsConnected() ) ? true : Logon();
}

void Database::Logoff()
{
	if( m_pMysql != NULL )
	{
		mysql_close( m_pMysql );
		m_pMysql = NULL;
	}
}

bool Database::IsConnected()
{
	if( m_pMysql == NULL ) return false;
	return ( mysql_ping( m_pMysql ) == 0 );
}

my_ulonglong Database::GetInsertId()
{
    if( m_pMysql == NULL ) return 0;
    return mysql_insert_id(m_pMysql);
}

bool Recordset::MoveNext()
{
	m_nIndex = 0x00;
	m_MysqlRow = mysql_fetch_row( m_pMysqlRes );
	return ( m_MysqlRow != NULL );
}

bool Recordset::operator >> ( unsigned long long& nValue )
{
	if( ( m_MysqlRow == NULL ) || ( m_nIndex >= m_NumFields ) )
		return false;

	const char* szRow = m_MysqlRow[m_nIndex ++];
	if( szRow == NULL ) return false;

	nValue = boost::lexical_cast<unsigned long long>( szRow );
	return true;
}

bool Recordset::operator >> ( unsigned int& nValue )
{
	if( ( m_MysqlRow == NULL ) || ( m_nIndex >= m_NumFields ) )
		return false;

	const char* szRow = m_MysqlRow[m_nIndex ++];
	if( szRow == NULL ) return false;

	nValue = boost::lexical_cast<unsigned int>( szRow );
	return true;
}

bool Recordset::operator >> ( unsigned short& nValue )
{
	if( ( m_MysqlRow == NULL ) || ( m_nIndex >= m_NumFields ) )
		return false;

	const char* szRow = m_MysqlRow[m_nIndex ++];
	if( szRow == NULL ) return false;

	nValue = boost::lexical_cast<unsigned short>( szRow );
	return true;
}

bool Recordset::operator >> ( int& nValue )
{
	if( ( m_MysqlRow == NULL ) || ( m_nIndex >= m_NumFields ) )
		return false;

	const char* szRow = m_MysqlRow[m_nIndex ++];
	if( szRow == NULL ) return false;

	nValue = boost::lexical_cast<int>( szRow );
	return true;
}

bool Recordset::operator >> ( short& nValue )
{
	if( ( m_MysqlRow == NULL ) || ( m_nIndex >= m_NumFields ) )
		return false;

	const char* szRow = m_MysqlRow[m_nIndex ++];
	if( szRow == NULL ) return false;
	nValue = boost::lexical_cast<short>( szRow );
	return true;
}

bool Recordset::operator >> ( unsigned char& nValue )
{
	if( ( m_MysqlRow == NULL ) || ( m_nIndex >= m_NumFields ) )
		return false;

	const char* szRow = m_MysqlRow[m_nIndex ++];
	if( szRow == NULL ) return false;
	nValue = boost::lexical_cast<unsigned char>( szRow );
	return true;
}

bool Recordset::operator >> ( char& nValue )
{
	if( ( m_MysqlRow == NULL ) || ( m_nIndex >= m_NumFields ) )
		return false;

	const char* szRow = m_MysqlRow[m_nIndex ++];
	if( szRow == NULL ) return false;
	nValue = boost::lexical_cast<char>( szRow );
	return true;
}

bool Recordset::operator >> ( std::string& strValue )
{
	if( ( m_MysqlRow == NULL ) || ( m_nIndex >= m_NumFields ) )
		return false;

	const char* szRow = m_MysqlRow[m_nIndex ++];

	if( szRow == NULL ) return false;
	strValue = szRow;
	return true;
}

bool Recordset::NextField()
{
	if ((m_MysqlRow == NULL) || (m_nIndex >= m_NumFields))
		return false;

	const char* szRow = m_MysqlRow[m_nIndex++];

	if (szRow == NULL){
		return false;
	}
	else
    	return true;

}

/////////////////////////////////////////////////////////////////////////////

bool Database::Logon()
{
    static std::mutex s_dbInitLock;
	s_dbInitLock.lock();
	Logoff();
	m_pMysql = mysql_init( NULL );
	s_dbInitLock.unlock();

	if( m_pMysql == NULL ) return false;

	MYSQL* pMysql = mysql_real_connect( m_pMysql, m_DBParam.strHost.c_str(),
		m_DBParam.strLogin.c_str(), m_DBParam.strPasswd.c_str(),
		m_DBParam.strDefaultDB.c_str(), m_DBParam.nPort, NULL, CLIENT_MULTI_RESULTS );
    
    if (pMysql)
    {
        mysql_set_character_set(pMysql, "utf8");
    }
	else
	{
		printf("Fail to connect to mysql: %s", mysql_error(m_pMysql));
	}

	return ( pMysql != NULL ) ? ( mysql_autocommit( pMysql, 1 ) == 0 ) : false;
}

my_ulonglong Database::GetAffectedRows()
{
// 	my_ulonglong nNumRows = 0x00;
// 	if( m_pMysql != NULL )
// 	{
// 		nNumRows = mysql_affected_rows( m_pMysql );
// 	}
	return numAffectedRows;
}

std::shared_ptr<Recordset> Database::Query( const char* szSQL, int nCommit )
{
	RecordsetPtr pRecordset;
	const char* pError = NULL;

	if( mysql_query( m_pMysql, szSQL ) == 0 )
	{
		numAffectedRows = mysql_affected_rows(m_pMysql);
		MYSQL_RES* pMysqlRes = mysql_store_result( m_pMysql );
		while( mysql_next_result( m_pMysql ) == 0x00 );

		if( pMysqlRes != NULL )
		{
			my_ulonglong nNumRows = mysql_num_rows( pMysqlRes );
			if( nNumRows > 0x00 ) pRecordset.reset(new Recordset( pMysqlRes ));
			else if( pMysqlRes != NULL ) mysql_free_result( pMysqlRes );
			if( nCommit ) mysql_commit( m_pMysql );
		}
	}
	else
	{
		numAffectedRows = 0;
		pError = mysql_error( m_pMysql );
        if (pError)
        {
            std::cout << "DBError:" << pError << " SQL:" << szSQL << "\n";
        }
	}
	return pRecordset;
}

/////////////////////////////////////////////////////////////////////////////

void DBRequestQueue::DBWorkThread(DBRequestQueue* pDBQueue, MYSQL_PARAM dbParam)
{
    Database db;
    db.SetDBConfig(dbParam);

    while (true)
    {
        std::shared_ptr<DBRequest> request(pDBQueue->GetRequest());
        if( request.get() == NULL )
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;			
        }

        while (!db.GetConnection())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        request->OnRequest(db);

        pDBQueue->FinishRequest(request);
    }
}

void DBRequestQueue::Init(int nThread, MYSQL_PARAM dbParam)
{
    for (int i=0; i<nThread; i++)
    {
        std::shared_ptr<std::thread> th(new std::thread(std::bind(DBRequestQueue::DBWorkThread, this, dbParam)));
        m_WorkThread.push_back(th);
    }
}

void DBRequestQueue::AddQueueMsg(std::shared_ptr<DBRequest> request)
{
    m_TempQueue.push_back(request);
}

int DBRequestQueue::Update()
{
    if (!m_TempQueue.empty())
    {
        m_WorkMutex.lock();
        m_WorkQueue.insert(m_WorkQueue.end(), m_TempQueue.begin(), m_TempQueue.end());
        m_WorkMutex.unlock();
        m_TempQueue.clear();
    }

    DBRequestList finishQueue;
    m_FinishMutex.lock();
    m_FinishQueue.swap(finishQueue);
    m_FinishMutex.unlock();
    
    while (!finishQueue.empty())
    {
        std::shared_ptr<DBRequest> request = finishQueue.front();
        finishQueue.pop_front();

        request->OnFinish();
    }

    return 0;
}

std::shared_ptr<DBRequest> DBRequestQueue::GetRequest()
{
    std::shared_ptr<DBRequest> request;
    m_WorkMutex.lock();
    if (!m_WorkQueue.empty())
    {
        request = m_WorkQueue.front();
        m_WorkQueue.pop_front();
    }
    m_WorkMutex.unlock();

    return request;
}

void DBRequestQueue::FinishRequest(std::shared_ptr<DBRequest> request)
{
    m_FinishMutex.lock();
    m_FinishQueue.push_back(request);
    m_FinishMutex.unlock();
}
 
