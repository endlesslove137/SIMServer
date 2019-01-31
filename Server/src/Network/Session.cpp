#include "Session.h"
#include "IoHandle.h"
#include "SendBuf.h"

#include<iomanip>
#include<fstream>
using namespace std;

Session::Session(unsigned int nIndex, IoHandle* ioHandle): m_Socket(*(ioHandle->m_IoServicePtr)), m_Strand(*(ioHandle->m_IoServicePtr))
{
    m_nIndex = nIndex;
    m_IoHandle = ioHandle;

    m_pSendBuffer = new SendBuffer;
    m_pRecvBuffer = new RecvBuffer;
    m_pSendBuffer->Create(m_IoHandle->m_Config.nMaxSendBuf, m_IoHandle->m_Config.nMaxClientPack, m_IoHandle->m_Config.nRecvOffset);
    m_pRecvBuffer->Create(m_IoHandle->m_Config.nMaxRecvBuf, m_IoHandle->m_Config.nMaxClientPack, m_IoHandle->m_Config.nRecvOffset);

    memset(&m_ConnectAddrInfo, 0, sizeof(m_ConnectAddrInfo));

    Init();
}

Session::~Session()
{
    delete m_pSendBuffer;
    delete m_pRecvBuffer;
}

ConnectAddrInfo& Session::GetConnectAddrInfo()
{
    return m_ConnectAddrInfo;
}

void Session::BindNetObject(NetObject* pNetObject)
{
    m_NetObject = pNetObject;
    if (m_NetObject)
    {
        m_NetObject->m_Session = this;
    }
}

void Session::Init()
{
    m_NetObject = NULL;
    m_pSendBuffer->Clear();
    m_pRecvBuffer->Clear();
    m_bRemove = false;
    m_WaitPreRecv = false;
    m_DisconnectOrdered = false;
    m_LastRecvBufLen = 0;
    m_LastSyncTime = clock();
    m_LastSendTime = m_LastSyncTime;
    m_WaitSendListSize = 0;
    m_WaitSendList.clear();
}

void Session::Disconnect(bool bForceDisconnect)
{
    m_IoHandle->CloseNetObject(GetIndex(), bForceDisconnect);
}

void Session::RealDisconnect(bool bForceDisconnect)
{
    if (bForceDisconnect)
    {
        Remove();
    }
    else
    {
        m_DisconnectOrdered = true;
    }
}

bool Session::Send(const char* pMsg, unsigned int nSize)
{
    m_IoHandle->Send(GetIndex(), pMsg, nSize);
    return true;
}

void Session::OnAccept()
{
    m_NetObject->OnAccept(this->GetIndex());
    if (PreRecv() != true)
    {        
        printf("Session::OnAccept %d %d %d \n", GetIndex(), GetRecvBuffer()->m_bComplete, GetRecvBuffer()->m_pQueue->GetSpace());
        RealDisconnect(true);
    }
}

bool Session::PreRecv()
{
    char* pRecvBuf	= NULL;
    int nRecvSize	= 0;

    GetRecvBuffer()->GetRecvParam((char**)&pRecvBuf, (int&)nRecvSize);

    if (nRecvSize == 0)
    {
        return false;
    }

    m_Socket.async_read_some(boost::asio::buffer(pRecvBuf, nRecvSize),
        /*m_Strand.wrap(*/
        std::bind(&Session::handle_read, this,
        std::placeholders::_1,
        std::placeholders::_2, pRecvBuf, nRecvSize)/*)*/);

    return true;
}

void Session::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred, char* p, int n)
{
    if (!e)
    {
        do 
        {
            if (bytes_transferred == 0)
            {
                m_IoHandle->CloseNetObject(GetIndex(), true);
                break;
            }

            GetRecvBuffer()->Completion(bytes_transferred);
            if (PreRecv() == false)
            {
                m_WaitPreRecv = true;
            }
        } while (0);
    }
    else
    {
#ifdef _WIN32
        if (e.value() != 2
            && e.value() != 10054
            && e.value() != 995
            && e.value() != 10053
            && e.value() != 1236
            && e.value() != 121)
        {
            printf("RecvError: %d %s Complete:%d DataPtr:%p Tail:%d Head:%d Ptr:%p Size:%d \n", 
                e.value(), 
                e.message().c_str(),
                GetRecvBuffer()->m_bComplete, 
                GetRecvBuffer()->m_pQueue->m_pData, 
                GetRecvBuffer()->m_pQueue->m_nTail,
                GetRecvBuffer()->m_pQueue->m_nHead,
                p, n);
        }
#endif

        m_IoHandle->CloseNetObject(GetIndex(), true);
    }
}

bool Session::PreSend()
{
    char* pSendBuf	= NULL;
    int nSendSize	= 0;

    if (GetSendBuffer()->GetSendParam((char**)&pSendBuf, (int&)nSendSize ) == false)
    {
        return true;
    }

    if (nSendSize == 0)
    {
        return true;
    }

    m_Socket.async_write_some(boost::asio::buffer(pSendBuf, nSendSize),
        /*m_Strand.wrap(*/
        std::bind(&Session::handle_write, this,
        		std::placeholders::_1,
        std::placeholders::_2, pSendBuf, nSendSize)/*)*/);

    return true;
}

bool Session::PreSendEx(std::size_t bytes_transferred)
{
    GetSendBuffer()->m_pQueue->Dequeue(NULL, bytes_transferred);
    if (GetSendBuffer()->m_pQueue->GetLength() == 0)
    {
        GetSendBuffer()->m_bComplete = true;
    }
    else
    {
        char* pSendBuf = GetSendBuffer()->m_pQueue->GetReadPtr();
        int nSendSize = GetSendBuffer()->m_pQueue->GetReadableLen();

        if (nSendSize == 0)
        {
            GetSendBuffer()->m_bComplete = true;
            return true;
        }

        m_Socket.async_write_some(boost::asio::buffer(pSendBuf, nSendSize),
            /*m_Strand.wrap(*/
            std::bind(&Session::handle_write, this,
            std::placeholders::_1,
            std::placeholders::_2, pSendBuf, nSendSize)/*)*/);
    }

    return true;
}

void Session::handle_write(const boost::system::error_code& e,
    std::size_t bytes_transferred, char* p, int n)
{
    if (!e)
    {
//         GetSendBuffer()->Completion(bytes_transferred);

        PreSendEx(bytes_transferred);
    }
    else
    {
#ifdef _WIN32
        if (e.value() != 2
            && e.value() != 10054
            && e.value() != 995
            && e.value() != 10053
            && e.value() != 1236
            && e.value() != 121)
        {
            printf("SendError: %d %s Complete:%d DataPtr:%p Tail:%d Head:%d Ptr:%p Size:%d \n", 
                e.value(), 
                e.message().c_str(),
                GetSendBuffer()->m_bComplete, 
                GetSendBuffer()->m_pQueue->m_pData, 
                GetSendBuffer()->m_pQueue->m_nTail,
                GetSendBuffer()->m_pQueue->m_nHead,
                p, n);
        }
#endif

        m_IoHandle->CloseNetObject(GetIndex(), true);
    }
}

void Session::RealSendData(const char* data, unsigned int len)
{
    if (HasDisconnectOrdered())
    {
        return;
    }

    if (!m_WaitSendList.empty())
    {
        std::vector<char> buf(data, data+len);
        m_WaitSendListSize++;
        m_WaitSendList.push_back(std::make_pair(0, buf));
        return;
    }

    if (GetSendBuffer()->GetSpace() < len)
    {
        std::vector<char> buf(data, data+len);
        m_WaitSendListSize++;
        m_WaitSendList.push_back(std::make_pair(0,buf));
        return;
    }

    GetSendBuffer()->Write((char*)data, len);
    //PreSend();
}

void Session::OnProcess()
{
    unsigned int nCurTime = clock();

    if (HasDisconnectOrdered())
    {
        if (m_WaitSendList.empty() && GetSendBuffer()->GetLength() == 0)
        {
            Remove();
        }
    }
    else
    {
        unsigned int nReadDataLen = 0;
        char* pReadPtr = GetRecvBuffer()->GetReadDataPtr(nReadDataLen);
		if (!pReadPtr)
		{
			ofstream f1;
			f1.open(".\\errlog.txt", ios::in | ios::out | ios_base::app);
			f1 << "Session::OnProcess1:" << nReadDataLen<< endl;
			f1.close();
			RealDisconnect(true);
		}

        if (nReadDataLen != 0 
            && nReadDataLen != m_LastRecvBufLen)
        {
			if (!pReadPtr)
			{
				ofstream f2;
				f2.open(".\\errlog2.txt", ios::in | ios::out | ios_base::app);
				f2 << "Session::OnProcess2:" << nReadDataLen << endl;
				f2.close();
				RealDisconnect(true);
			}
            unsigned int nProcessData = m_NetObject->OnRecv(pReadPtr, nReadDataLen);

            if (nProcessData <= nReadDataLen)
            {
                m_LastSyncTime = nCurTime;

                GetRecvBuffer()->RemoveReadData(nProcessData);
                m_LastRecvBufLen = nReadDataLen - nProcessData;
            }
            else
            {
                printf("Session::OnProcess %d %d\n", nProcessData, nReadDataLen);
                RealDisconnect(true);
            }
        }

        if (m_WaitPreRecv == true)
        {
            m_WaitPreRecv = false;
            if (PreRecv() == false)
            {
                m_WaitPreRecv = true;
            }
        }
    }

    {
        while (!m_WaitSendList.empty())
        {
            unsigned int nSpace = GetSendBuffer()->GetSpace();
            if (nSpace == 0)
            {
                break;
            }

            std::pair< unsigned int, std::vector<char> >& item = m_WaitSendList.front();
            std::vector<char>& buf = item.second;

            unsigned int nDataLen = buf.size() - item.first;
            if (nDataLen <= nSpace)
            {
                GetSendBuffer()->Write((char*)&buf[item.first], nDataLen);
                m_WaitSendList.pop_front();
                m_WaitSendListSize--;
            }
            else
            {
                GetSendBuffer()->Write((char*)&buf[item.first], nSpace);
                item.first += nSpace;
            }            
        }

        if ((nCurTime>m_LastSendTime) 
            && (nCurTime - m_LastSendTime > 100))
        {
            m_LastSendTime = nCurTime;
            PreSend();
        }
    }

    if (m_IoHandle->m_Config.nTimeOut != 0)
    {
        if ((nCurTime > m_LastSyncTime) && ((nCurTime - m_LastSyncTime) > m_IoHandle->m_Config.nTimeOut))
        {
            Remove();
        }
    }

    if (m_WaitSendListSize > 100000)
    {
        Remove();
    }
}
