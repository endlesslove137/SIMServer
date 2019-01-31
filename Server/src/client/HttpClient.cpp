#include "HttpClient.h"
//#include "log_wrap.h"
#include <stdio.h>
#include <string.h>


#include<iomanip>
#include<fstream>
using namespace std;


HttpClient::HttpClient() :m_nIndex(0)
{

}

HttpClient::~HttpClient()
{

}

void HttpClient::OnAccept(unsigned int nNetworkIndex)
{
	m_nIndex = nNetworkIndex;
	printf("http client accepted, %d", nNetworkIndex);
//	WRITE_DBG_LOG("http client accepted, %d", nNetworkIndex);
}

void HttpClient::OnDisconnect()
{
	printf("http client disconnected, %d", m_nIndex);
//	WRITE_DBG_LOG("http client disconnected, %d", m_nIndex);
}

unsigned int HttpClient::OnRecv(char *pMsg, unsigned int nSize)
{
	if (!pMsg)
	{
		ofstream f3;
		f3.open(".\\errlog3.txt", ios::in | ios::out | ios_base::app);
		f3 << "HttpClient::OnRecv nSize:" << nSize << endl;
		f3.close();
		return nSize;
	}

	char* str = new char[nSize + 1];
	memcpy(str, pMsg, nSize);
	str[nSize] = '\0';
	printf("%s", str);
//	WRITE_INFO_LOG("%s", str);
	delete []str;
	return nSize;
}

void HttpClient::OnConnect(bool bSuccess)
{
	
}

