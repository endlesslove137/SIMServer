#ifndef __CLIENT_MANAGER_H__
#define __CLIENT_MANAGER_H__

#include "Network/Network.h"
#include "ClientSocket.h"
#include "HttpClient.h"

NetObject* createClientSocket();
void destroyClientSocket(NetObject *pNetworkObject);
void realDeleteClientSocket(NetObject *pNetworkObject);

void addClient(ClientSocket* client);
void removeClient(int index);
ClientSocket* getClientByIndex(const int index);

void addLoggedinClient(const std::string& deviceID, ClientSocket* client);
void removeLoggedinClient(const std::string& deviceID);
ClientSocket* getClientByDeviceID(const std::string& deviceID);

NetObject* createHttpClient();
void destroyHttpClient(NetObject* pNetworkObject);
void realDeleteHttpClient(NetObject* pNetworkObject);

#endif	//__CLIENT_MANAGER_H__
