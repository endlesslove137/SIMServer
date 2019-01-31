#pragma once

#include "Network.h"
#include "Session.h"
#include <list>
#include <thread>
#include <memory>
#include <functional>
class IoHandle
{
public:
    IoHandle();

    bool Init(AsioNetwork::Config config);
    void Cleanup();
    bool StartListen(char *pIP, unsigned short nPort);
    void Send(unsigned int nIndex, const char* pMsg, unsigned int nSize);
    void SendToAll(const char* pMsg, unsigned int nSize);
    void BroadCast(const char* pBuf, unsigned int nLen, unsigned int* pIDs, unsigned int nCount);
    void BroadCastEx(const char* pBuf, unsigned int nLen, unsigned int* pIDs, unsigned int nCount);

    void CloseNetObject(unsigned int nIndex, bool bForceDisconnect);
    void CloseAllClient();
    unsigned int GetSessionAmount();
    unsigned int GetCurrentNetObjAmount();
    unsigned int GetCurrentSessionAmount();

    inline unsigned int ConvNetworkId(unsigned int nIndex)
    {
        if (nIndex < m_Config.nStartId || nIndex >= (m_Config.nStartId + m_Config.nMaxClient))
        {
            return 0;
        }

        return nIndex - m_Config.nStartId + 1;
    }

    unsigned int LogicThread();

    bool Connect(NetObject* pNetObj, char* pIP, unsigned short nPort);

private:
    void OnConnect(Session* pSession, bool bSuccess);
    void OnAccept(Session* pSession);
    void RealCloseNetObject(unsigned int nNetworkIndex, bool bForceDisconnect);    
    void RealCloseAllClient();    
    void CloseSession(Session* pSession);
    void PreAccept(const std::shared_ptr<boost::asio::ip::tcp::acceptor>& accepter);

    typedef std::list<Session*> SessionList;
    std::vector<Session*> m_ActiveSession;
    SessionList m_FreeSession;
    std::list< std::shared_ptr<boost::asio::ip::tcp::acceptor> > m_WaitAccept;

    SessionList m_UseSession;
    unsigned int m_SessionAmount;
    unsigned int m_AllocSessionAmount;
    unsigned int m_AllocNetObjectAmount;

    std::list< std::pair<unsigned int, Session*> > m_WaitFreeSession;

    std::vector< std::shared_ptr<boost::asio::ip::tcp::acceptor> > m_Acceptors;

    std::vector< std::shared_ptr<std::thread> > m_IoThreads;
    std::thread* m_LogicThread;
    boost::asio::io_service::work*	m_IoWorkPtr;
    boost::asio::io_service::work*	m_LogicWorkPtr;

    /// Handle completion of a connect operation.
    void handle_accept(Session *pSession, std::shared_ptr<boost::asio::ip::tcp::acceptor> accepter, const boost::system::error_code& e);
    void handle_connect(Session* pSession, const boost::system::error_code& e);

public:
    AsioNetwork::Config m_Config;
    boost::asio::io_service*	m_IoServicePtr;
    boost::asio::io_service*	m_LogicServicePtr;
};
