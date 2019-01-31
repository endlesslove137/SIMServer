#include "Network.h"
#include "Session.h"
#include "IoHandle.h"

//================================================================

NetObject::NetObject()
{
    m_Session = NULL;
}

NetObject::~NetObject()
{

}

void NetObject::Disconnect(bool bForceDisconnect/* = false*/)
{
    if (m_Session)
    {
        m_Session->Disconnect(bForceDisconnect);
    }
}

bool NetObject::Send(const char* pMsg, unsigned int nSize)
{
    if (nSize == 0)
    {
        return true;
    }

    if (m_Session)
    {
        return m_Session->Send(pMsg, nSize);
    }

    return false;
}

ConnectAddrInfo NetObject::GetConnectAddrInfo()
{
    if (m_Session)
    {
        return m_Session->GetConnectAddrInfo();
    }

    return ConnectAddrInfo();
}

//================================================================

bool AsioNetwork::Init(Config config)
{
    m_IoHandle = new IoHandle;
    return m_IoHandle->Init(config);
}

void AsioNetwork::Cleanup()
{
    m_IoHandle->Cleanup();

    delete m_IoHandle;
    m_IoHandle = NULL;
}

bool AsioNetwork::StartListen(char *pIP, unsigned short nPort)
{
    if (m_IoHandle)
    {
        return m_IoHandle->StartListen(pIP, nPort);
    }

    return false;
}

void AsioNetwork::CloseClient(unsigned int nIndex, bool bForceDisconnect)
{
    if (m_IoHandle)
    {
        m_IoHandle->CloseNetObject(nIndex, bForceDisconnect);
    }
}

void AsioNetwork::CloseAllClient()
{
    if (m_IoHandle)
    {
        m_IoHandle->CloseAllClient();
    }
}

bool AsioNetwork::Send(unsigned int nIndex, const char* pMsg, unsigned int nSize)
{
    if (m_IoHandle)
    {
        m_IoHandle->Send(nIndex, pMsg, nSize);
    }
    return true;
}

bool AsioNetwork::SendToAll(const char* pMsg, unsigned int nSize)
{
    if (m_IoHandle)
    {
        m_IoHandle->SendToAll(pMsg, nSize);
    }

    return true;
}

bool AsioNetwork::BroadCast(const char* pBuf, unsigned int nLen, unsigned int* pIDs, unsigned int nCount)
{
    if (m_IoHandle)
    {
        m_IoHandle->BroadCast(pBuf, nLen, pIDs, nCount);
    }

    return true;
}

bool AsioNetwork::BroadCastEx(const char* pBuf, unsigned int nLen, unsigned int* pIDs, unsigned int nCount)
{
    if (m_IoHandle)
    {
        m_IoHandle->BroadCastEx(pBuf, nLen, pIDs, nCount);
    }

    return true;
}

bool AsioNetwork::IsValidateID(unsigned int nID)
{
    if (m_IoHandle)
    {
        return m_IoHandle->ConvNetworkId(nID) != 0;
    }

    return true;
}

unsigned int AsioNetwork::GetCurrentClientAmount()
{
    if (m_IoHandle)
    {
        return m_IoHandle->GetSessionAmount();
    }

    return 0;
}

unsigned int AsioNetwork::GetCurrentNetObjAmount()
{
    if (m_IoHandle)
    {
        return m_IoHandle->GetCurrentNetObjAmount();
    }

    return 0;
}

unsigned int AsioNetwork::GetCurrentSessionAmount()
{
    if (m_IoHandle)
    {
        return m_IoHandle->GetCurrentSessionAmount();
    }

    return 0;
}

unsigned int AsioNetwork::Update()
{
    if (m_IoHandle)
    {
        return m_IoHandle->LogicThread();
    }

    return 0;
}

bool AsioNetwork::Connect(NetObject* pNetObj, char* pIP, unsigned short nPort)
{
    if (m_IoHandle)
    {
        return m_IoHandle->Connect(pNetObj, pIP, nPort);
    }

    return false;
}
