#ifndef __NETWORK_H____
#define __NETWORK_H____

class ConnectAddrInfo
{
public:
    ConnectAddrInfo()
    {
        nSrcIp = 0;
        nSrcPort = 0;
        nDstIp = 0;
        nDstPort = 0;
    }

    unsigned int nSrcIp;
    unsigned short nSrcPort;
    unsigned int nDstIp;
    unsigned short nDstPort;
};

class Session;
class NetObject
{
public:
    NetObject();
    virtual ~NetObject();

    void Disconnect(bool bForceDisconnect = false);
    bool Send(const char* pMsg, unsigned int nSize);
    ConnectAddrInfo GetConnectAddrInfo();

    virtual void OnAccept(unsigned int nNetworkIndex){}
    virtual void OnDisconnect(){}
    virtual	unsigned int OnRecv(char *pMsg, unsigned int nSize) = 0;
    virtual void OnConnect(bool bSuccess){}

    Session* m_Session;
};

typedef NetObject* (*fnCallBackCreateAcceptedObject)();
typedef void (*fnCallBackDestroyAcceptedObject)(NetObject *pNetworkObject);
typedef void (*fnCallBackRealDelAcceptedObject)(NetObject *pNetworkObject);

class IoHandle;
class AsioNetwork
{
public:
    struct Config
    {
        unsigned int nRecvOffset;
        unsigned int nStartId;
        unsigned int nMaxClient;
        unsigned int nMaxSendBuf;
        unsigned int nMaxRecvBuf;
        unsigned int nMaxClientPack;
        unsigned int nIoThreads;
        unsigned int nTimeOut;
        unsigned int nPreAcceptAmount;
        fnCallBackCreateAcceptedObject fnCreateNetObject;
        fnCallBackDestroyAcceptedObject fnDestroyNetObject;
        fnCallBackRealDelAcceptedObject fnRealDelNetObject;
    };

public:
    bool Init(Config config);
    void Cleanup();

    bool StartListen(char *pIP, unsigned short nPort);
    void CloseClient(unsigned int nIndex, bool bForceDisconnect = false);
    void CloseAllClient();

    bool Send(unsigned int nIndex, const char* pMsg, unsigned int nSize);
    bool SendToAll(const char* pMsg, unsigned int nSize);
    bool BroadCast(const char* pBuf, unsigned int nLen, unsigned int* pIDs, unsigned int nCount);
    bool BroadCastEx(const char* pBuf, unsigned int nLen, unsigned int* pIDs, unsigned int nCount);
    bool IsValidateID(unsigned int nID);
    unsigned int GetCurrentClientAmount();
    unsigned int GetCurrentNetObjAmount();
    unsigned int GetCurrentSessionAmount();
    
    unsigned int Update();

    bool Connect(NetObject* pNetObj, char* pIP, unsigned short nPort);

    IoHandle* m_IoHandle;
};

#endif //__NETWORK_H____
