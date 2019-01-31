#include "IoHandle.h"
#include <functional>
#include <boost/asio.hpp>
#include <thread>
#include <functional>
#include <memory>
#include "database/Table.h"
#include "client/ClientManager.h"
IoHandle::IoHandle()
{

}

bool IoHandle::Init(AsioNetwork::Config config)
{
    m_Config = config;

    m_IoServicePtr	= new boost::asio::io_service;
    m_IoWorkPtr	= new boost::asio::io_service::work(*m_IoServicePtr);

    m_LogicServicePtr = new boost::asio::io_service;
    m_LogicWorkPtr	= new boost::asio::io_service::work(*m_LogicServicePtr);

    for (unsigned int i = 0; i < m_Config.nIoThreads; i++)
    {
    	std::shared_ptr<std::thread> thread (new std::thread( std::bind(static_cast<std::size_t (boost::asio::io_service::*)()>
    	(&boost::asio::io_service::run), m_IoServicePtr)));
        m_IoThreads.push_back(thread);
    }

    //m_LogicThread = new boost::thread(boost::bind(&IoHandle::LogicThread, this));

    m_ActiveSession.push_back(NULL);

    for (unsigned int i = 0; i < m_Config.nMaxClient; i++)
    {
        Session* pSession = new Session(i + m_Config.nStartId, this);
        m_FreeSession.push_back(pSession);

        m_ActiveSession.push_back(NULL);
    }

    m_SessionAmount = 0;
    m_AllocSessionAmount = 0;
    m_AllocNetObjectAmount = 0;

    return true;
}

unsigned int IoHandle::GetSessionAmount()
{
    return m_SessionAmount;
}

unsigned int IoHandle::GetCurrentNetObjAmount()
{
    return m_AllocNetObjectAmount;
}

unsigned int IoHandle::GetCurrentSessionAmount()
{
    return m_AllocSessionAmount;
}

void IoHandle::Cleanup()
{
    m_IoServicePtr->stop();
    m_LogicServicePtr->stop();
    for (int i = 0; i < int(m_IoThreads.size()); i++)
    {
        m_IoThreads[i]->join();
    }

    //m_LogicThread->join();
    //delete m_LogicThread;

    m_Acceptors.clear();

    delete m_IoWorkPtr;
    delete m_LogicWorkPtr;

    delete m_IoServicePtr;
    delete m_LogicServicePtr;
}

bool IoHandle::StartListen(char *pIP, unsigned short nPort)
{
    std::shared_ptr<boost::asio::ip::tcp::acceptor> accepter(new boost::asio::ip::tcp::acceptor(*m_IoServicePtr));

    boost::asio::ip::address adrs(boost::asio::ip::address::from_string(pIP));
    boost::asio::ip::tcp::endpoint endpoint(adrs, nPort);
    accepter->open(endpoint.protocol());
    accepter->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    accepter->bind(endpoint);
    accepter->listen();

    m_Acceptors.push_back(accepter);

    for (unsigned int i = 0; i < m_Config.nPreAcceptAmount; i++)
    {
        PreAccept(accepter);
    }

    return true;
}

void IoHandle::PreAccept(const std::shared_ptr<boost::asio::ip::tcp::acceptor>& accepter)
{
    if (m_FreeSession.empty())
    {
        m_WaitAccept.push_back(accepter);
        return;
    }

    Session *pNewAccept = m_FreeSession.front();
    m_FreeSession.pop_front();

    m_AllocSessionAmount++;

    if (pNewAccept == NULL)
    {
        printf("IoHandle::PreAccept\n");
        m_FreeSession.push_back(pNewAccept);
        return;
    }

    accepter->async_accept(pNewAccept->m_Socket,
        std::bind(&IoHandle::handle_accept, this, pNewAccept, accepter,
//        boost::asio::placeholders::error));
        		std::placeholders::_1));
}

void IoHandle::handle_accept(Session *pSession, std::shared_ptr<boost::asio::ip::tcp::acceptor> accepter, const boost::system::error_code& e)
{
    if (!e)
    {
        try
        {
            pSession->GetConnectAddrInfo().nSrcIp = ntohl(pSession->m_Socket.local_endpoint().address().to_v4().to_ulong());
            pSession->GetConnectAddrInfo().nSrcPort = pSession->m_Socket.local_endpoint().port();
            pSession->GetConnectAddrInfo().nDstIp = ntohl(pSession->m_Socket.remote_endpoint().address().to_v4().to_ulong());
            pSession->GetConnectAddrInfo().nDstPort = pSession->m_Socket.remote_endpoint().port();

            m_LogicServicePtr->post(std::bind(&IoHandle::OnAccept, this, pSession));
        }
        catch (boost::system::system_error &ec)
        {
            m_LogicServicePtr->post(std::bind(&IoHandle::CloseSession, this, pSession));
            printf("IoHandle::handle_accept %s\n", ec.what());
        }
    }
    else
    {
        m_LogicServicePtr->post(std::bind(&IoHandle::CloseSession, this, pSession));
    }

    m_LogicServicePtr->post(std::bind(&IoHandle::PreAccept, this, accepter));
}

void IoHandle::OnAccept(Session* pSession)
{
    unsigned int nPrivateId = ConvNetworkId(pSession->GetIndex());
    if (m_ActiveSession[nPrivateId] != NULL)
    {
        printf("IoHandle::OnAccept %d\n", nPrivateId);
        CloseSession(pSession);
        return;
    }

    m_ActiveSession[nPrivateId] = pSession;
    m_UseSession.push_back(pSession);
    m_SessionAmount++;

    pSession->Init();
    pSession->BindNetObject(m_Config.fnCreateNetObject());

    m_AllocNetObjectAmount++;
    pSession->OnAccept();
}

bool IoHandle::Connect(NetObject* pNetObj, char* pIP, unsigned short nPort)
{
    if (m_FreeSession.empty())
    {
        return false;
    }

    Session *pNewSession = m_FreeSession.front();
    m_FreeSession.pop_front();

    m_AllocSessionAmount++;

    pNewSession->Init();
    pNewSession->BindNetObject(pNetObj);

    boost::asio::ip::address adrs(boost::asio::ip::address::from_string(pIP));
    boost::asio::ip::tcp::endpoint endpoint(adrs, nPort);

    pNewSession->m_Socket.async_connect(endpoint,
        std::bind(&IoHandle::handle_connect, this, pNewSession,
        std::placeholders::_1));

    return true;
}

void IoHandle::handle_connect(Session *pSession, const boost::system::error_code& e)
{
    if (!e)
    {
        try
        {
            pSession->GetConnectAddrInfo().nSrcIp = ntohl(pSession->m_Socket.local_endpoint().address().to_v4().to_ulong());
            pSession->GetConnectAddrInfo().nSrcPort = pSession->m_Socket.local_endpoint().port();
            pSession->GetConnectAddrInfo().nDstIp = ntohl(pSession->m_Socket.remote_endpoint().address().to_v4().to_ulong());
            pSession->GetConnectAddrInfo().nDstPort = pSession->m_Socket.remote_endpoint().port();

            m_LogicServicePtr->post(std::bind(&IoHandle::OnConnect, this, pSession, true));
        }
        catch (boost::system::system_error &ec)
        {
            m_LogicServicePtr->post(std::bind(&IoHandle::OnConnect, this, pSession, false));
            printf("IoHandle::handle_accept %s\n", ec.what());
        }
    }
    else
    {
        m_LogicServicePtr->post(std::bind(&IoHandle::OnConnect, this, pSession, false));
    }
}

void IoHandle::OnConnect(Session* pSession, bool bSuccess)
{
    if (bSuccess)
    {
        unsigned int nPrivateId = ConvNetworkId(pSession->GetIndex());
        if (m_ActiveSession[nPrivateId] != NULL)
        {
            printf("IoHandle::OnConnect %d\n", nPrivateId);
        }

        m_ActiveSession[nPrivateId] = pSession;
        m_UseSession.push_back(pSession);
        m_SessionAmount++;

        pSession->GetNetObject()->OnConnect(bSuccess);

        if (pSession->PreRecv() != true)
        {
            printf("IoHandle::OnConnect PreRecv %d %d %d \n", pSession->GetIndex(), pSession->GetRecvBuffer()->m_bComplete, pSession->GetRecvBuffer()->m_pQueue->GetSpace());
            pSession->RealDisconnect(true);
        }
    }
    else
    {
        pSession->GetNetObject()->OnConnect(bSuccess);

        pSession->BindNetObject(NULL);
        CloseSession(pSession);
    }
}


void IoHandle::Send(unsigned int nIndex, const char* pMsg, unsigned int nSize)
{
    unsigned int nPrivateId = ConvNetworkId(nIndex);
    if (nPrivateId == 0)
    {
        return;
    }

    Session* pSession = m_ActiveSession[nPrivateId];
    if (!pSession)
    {
        return;
    }

    pSession->RealSendData(pMsg, nSize);
}

void IoHandle::SendToAll(const char* pMsg, unsigned int nSize)
{
    SessionList::iterator it = m_UseSession.begin();
    while (it != m_UseSession.end())
    {
        Session* pSession = *it;
        it++;

        if (!pSession->ShouldBeRemoved())
        {
            pSession->RealSendData(pMsg, nSize);
        }
    }
}

void IoHandle::BroadCast(const char* pBuf, unsigned int nLen, unsigned int* pIDs, unsigned int nCount)
{
    for (unsigned int i=0; i<nCount; i++)
    {
        unsigned int nPrivateId = ConvNetworkId(pIDs[i]);
        Session* pSession = m_ActiveSession[nPrivateId];
        if (!pSession)
        {
            continue;
        }

        pSession->RealSendData(pBuf, nLen);
    }
}

void IoHandle::BroadCastEx(const char* pBuf, unsigned int nLen, unsigned int* pIDs, unsigned int nCount)
{
    for (unsigned int i=0; i<nCount; i++)
    {
        unsigned int nPrivateId = ConvNetworkId(pIDs[i]);
        Session* pSession = m_ActiveSession[nPrivateId];
        if (!pSession)
        {
            continue;
        }

        unsigned short nPackLen = nLen + sizeof(unsigned short);
        pSession->RealSendData((char*)&nPackLen, sizeof(nPackLen));
        pSession->RealSendData(pBuf, nLen);
    }
}

void IoHandle::CloseNetObject(unsigned int nIndex, bool bForceDisconnect)
{
    if (ConvNetworkId(nIndex) == 0)
    {
        return;
    }

    m_LogicServicePtr->post(std::bind(&IoHandle::RealCloseNetObject, this, nIndex, bForceDisconnect));
}

void IoHandle::CloseAllClient()
{
    m_LogicServicePtr->post(std::bind(&IoHandle::RealCloseAllClient, this));
}

void IoHandle::RealCloseNetObject(unsigned int nIndex, bool bForceDisconnect)
{
    unsigned int nPrivateId = ConvNetworkId(nIndex);
    Session* pSession = m_ActiveSession[nPrivateId];
    if (!pSession)
    {
        return;
    }

    pSession->RealDisconnect(bForceDisconnect);
}

void IoHandle::RealCloseAllClient()
{
    SessionList::iterator it = m_UseSession.begin();
    while (it != m_UseSession.end())
    {
        Session* pSession = *it;
        it++;

        pSession->RealDisconnect(true);
    }
}

unsigned int IoHandle::LogicThread()
{

//    while (!m_LogicServicePtr->stop())
    {
        {
            SessionList::iterator it = m_UseSession.begin();
            while (it != m_UseSession.end())
            {
                Session* pSession = *it;
                it++;
                if (pSession->ShouldBeRemoved())
                    continue;

                pSession->OnProcess();
            }
        }

        {
            SessionList temp;
            SessionList::iterator it = m_UseSession.begin();
            while (it != m_UseSession.end())
            {
                Session* pSession = *it;
                SessionList::iterator temp_it = it++;

                if (pSession->ShouldBeRemoved())
                {
                    temp.push_back(pSession);
                    unsigned int nPrivateId = ConvNetworkId(pSession->GetIndex());
                    m_ActiveSession[nPrivateId] = NULL;
                    m_UseSession.erase(temp_it);
                    m_SessionAmount--;
                }
            }

            it = temp.begin();
            while (it != temp.end())
            {
                Session* pSession = *it;
                it++;

                NetObject* pNetObject = pSession->GetNetObject();
                pNetObject->OnDisconnect();

                m_Config.fnDestroyNetObject(pNetObject);

                CloseSession(pSession);
            }
        }

        {
            std::list< std::pair<unsigned int, Session*> >::iterator it = m_WaitFreeSession.begin();
            while (it != m_WaitFreeSession.end())
            {
                std::list< std::pair<unsigned int, Session*> >::iterator temp_it = it++;
                //unsigned int nCurTime = GetTickCount();
                unsigned int nCurTime = clock();
                if (nCurTime < temp_it->first || ((nCurTime - temp_it->first) < 20*1000))
                {
                    break;
                }

                Session* pSession = temp_it->second;

                if (pSession->GetNetObject() != NULL)
                {
                    m_Config.fnRealDelNetObject(pSession->GetNetObject());
                    m_AllocNetObjectAmount--;
                }

                pSession->Init();
                m_FreeSession.push_back(pSession);
                if (!m_WaitAccept.empty())
                {
                    PreAccept(m_WaitAccept.front());
                    m_WaitAccept.pop_front();
                }

                m_AllocSessionAmount--;

                m_WaitFreeSession.erase(temp_it);
            }
        }

        std::size_t nAmount = m_LogicServicePtr->poll();
        return nAmount;
    }
}

void IoHandle::CloseSession(Session* pSession)
{

    pSession->m_Socket.close();

    m_WaitFreeSession.push_back(std::make_pair(clock(), pSession));
}
