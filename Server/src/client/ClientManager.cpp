

#include <map>

#include "ClientManager.h"
#include "database/Table.h"
NetObject* createClientSocket()
{
	return  new ClientSocket;
}


void destroyClientSocket(NetObject *pNetworkObject)
{

}

void realDeleteClientSocket(NetObject *pNetworkObject)
{
	delete pNetworkObject;
}


std::map<int, ClientSocket*> allClients;
std::map<std::string, ClientSocket*> loggedinClients;

void addClient(ClientSocket* client)
{
	if (client)
	{
		allClients[client->getIndex()] = client;
	}
}

void removeClient(int index)
{
	allClients.erase(index);
	//printf("user Login out Online:%d\n", allClients.size());
}

ClientSocket* getClientByIndex(const int index)
{
	auto iter = allClients.find(index);
	if (iter != allClients.end())
	{
		return iter->second;
	}
	return nullptr;
}

void addLoggedinClient(const std::string& deviceID, ClientSocket* client)
{
	if (client)
	{
		loggedinClients[deviceID] = client;
		//		printf("user Login in OnLine:%d\n", loggedinClients.size());
		printf("有新的用户登陆 目前在线总数为:%d\n", loggedinClients.size());
	}
}

void removeLoggedinClient(const std::string& deviceID)
{
	loggedinClients.erase(deviceID);
	printf("user Login out Online:%d\n", loggedinClients.size());
}

ClientSocket* getClientByDeviceID(const std::string& deviceID)
{
	auto iter = loggedinClients.find(deviceID);
	if (iter != loggedinClients.end())
	{
		return iter->second;
	}
	return nullptr;
}

NetObject* createHttpClient()
{
	return new HttpClient();
}

void destroyHttpClient(NetObject* pNetworkObject)
{

}

void realDeleteHttpClient(NetObject* pNetworkObject)
{
	delete pNetworkObject;
}
