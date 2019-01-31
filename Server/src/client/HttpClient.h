#pragma once

#include "Network/Network.h"

class HttpClient : public NetObject
{
public:
	HttpClient();
	virtual ~HttpClient();

	virtual void OnAccept(unsigned int nNetworkIndex) override;
	virtual void OnDisconnect();
	virtual	unsigned int OnRecv(char *pMsg, unsigned int nSize);
	virtual void OnConnect(bool bSuccess);

private:
	unsigned int m_nIndex;
};