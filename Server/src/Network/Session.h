#pragma once

#ifdef _WIN32
#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif
#endif

#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <list>
#include "Network.h"
#include "SendBuf.h"
#include "RecvBuf.h"

class IoHandle;
class Session
{
public:
    Session(unsigned int nIndex, IoHandle* ioHandle);
    ~Session();
    void Init();

    inline unsigned int     GetIndex() { return m_nIndex; }
    inline SendBuffer*		GetSendBuffer() { return m_pSendBuffer; }
    inline RecvBuffer*		GetRecvBuffer() { return m_pRecvBuffer; }
    inline void				Remove() { m_bRemove = true; }
    inline bool             ShouldBeRemoved() { return m_bRemove; }
    inline bool			    HasDisconnectOrdered() { return m_DisconnectOrdered; }
    inline NetObject*       GetNetObject() { return m_NetObject; }

    void Disconnect(bool bForceDisconnect);
    void RealDisconnect(bool bForceDisconnect);
    bool Send(const char* pMsg, unsigned int nSize);
    void OnAccept();
    void OnProcess();
    bool PreRecv();
    bool PreSend();
    bool PreSendEx(std::size_t bytes_transferred);

    void RealSendData(const char* data, unsigned int len);

    void BindNetObject(NetObject* pNetObject);

    ConnectAddrInfo& GetConnectAddrInfo();

private:
    unsigned int m_nIndex;
    NetObject* m_NetObject;
    IoHandle* m_IoHandle;
    ConnectAddrInfo m_ConnectAddrInfo;

    SendBuffer* m_pSendBuffer;
    RecvBuffer* m_pRecvBuffer;
    bool		m_bRemove;
    bool        m_WaitPreRecv;
    bool        m_DisconnectOrdered;
    unsigned int m_LastRecvBufLen;
    unsigned int m_LastSyncTime;
    unsigned int m_LastSendTime;
    unsigned int m_WaitSendListSize;
    std::list< std::pair< unsigned int, std::vector<char> > >	m_WaitSendList;

private:
    /// Handle completion of a read operation.
    void handle_read(const boost::system::error_code& e,
        std::size_t bytes_transferred, char* p, int n);

    /// Handle completion of a write operation.
    void handle_write(const boost::system::error_code& e,
        std::size_t bytes_transferred, char* p, int n);

public:
    /// Strand to ensure the connection's handlers are not called concurrently.
    boost::asio::io_service::strand m_Strand;

    /// Socket for the connection.
    boost::asio::ip::tcp::socket m_Socket;
};
