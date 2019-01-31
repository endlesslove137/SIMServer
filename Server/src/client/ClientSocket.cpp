#include "ClientSocket.h"
#include "GameConst.h"
#include "ClientManager.h"

#include "database/SQLRequest.h"
#include "utils/date.h"
#include<iostream>

#include<iomanip>
#include<fstream>
using namespace std;

extern AsioNetwork g_network;
extern DBRequestQueue g_DBQueue;

ClientSocket::ClientSocket() :index(0), pHead(NULL), _isConnected(false), heartbeatTimer(0), isKicked(false), BRank(false)
{
//	std::string dev_id = this->userInfo.deviceID;
	IMSI = "";

	
	
}

ClientSocket::~ClientSocket()
{
	pHead = NULL;


}

unsigned int ClientSocket::OnRecv( char *pMsg, unsigned int nSize )
{
	if (!pMsg)
	{
		ofstream f4;
		f4.open(".\\errlog4.txt", ios::in | ios::out | ios_base::app);
		f4 << "ClientSocket::OnRecv nSize:" << nSize << endl;
		f4.close();
		Disconnect(true);
		return nSize;
	}

	bool BCheck=false;
	unsigned int processSize(0);
	size_t headSize = sizeof(MessageHead);
	while (nSize > processSize)
	{
		if (pHead == NULL)
		{
			if (nSize - processSize >= headSize)
			{
				pHead = (MessageHead*)(pMsg + processSize);
				auto ntemp = sizeof(LoginMessage);
				//				if (userInfo.id == 0 && (pHead->command != CMD_C2S_LOGIN || pHead->packSize != ntemp))
				if (IMSI == "" && (pHead->command != SC_C2S_LOGIN && pHead->command != SC_C2S_SiM && pHead->command != C2S_getCN))
				{
					Disconnect(true);
					return nSize;
				}

				processSize += headSize;
			}
			else
			{
				return processSize;
			}
		}
		if (nSize - processSize >= pHead->packSize - headSize)
		{
			void* pFullMessage = pMsg + processSize - headSize;
			switch(pHead->command)
			{
				BCheck = true;
			case SC_C2S_LOGIN:
				processLoginMessage((PhoneMessage*)pFullMessage);
				break;
			case SC_C2S_SiM:
				processPhone((PhoneMessage*)pFullMessage);
				break;
			case C2S_Province:
				GetProvince((PCMessage*)pFullMessage);
				break;
			case C2S_getCN:
				GetCN();
				break;

				

			}
			processSize += pHead->packSize - headSize;
			pHead = NULL;
		}
		else	
		{
			return processSize;
		}
	}
	return processSize;
}

void ClientSocket::OnAccept( unsigned int nNetworkIndex )
{
//	printf("OnAccept %d\n", nNetworkIndex);
	index = nNetworkIndex;
	_isConnected = true;
	addClient(this);
	heartbeatTimer = utils::add_timer(1500*1000, std::bind(&ClientSocket::onHeartbeatTimer,
				this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void ClientSocket::OnDisconnect()
{
	//printf("OnDisconnect\n");
	removeClient(index);
	if (!isKicked && IMSI != "")
	{
		//SaveInfo
		//SaveUserInfo();
		removeLoggedinClient(IMSI);

	}
	utils::delete_timer(heartbeatTimer);
	heartbeatTimer = 0;
	_isConnected = false;
}

void ClientSocket::OnConnect( bool bSuccess )
{
//	printf("OnConnect\n");
}

bool ClientSocket::Send(EmptyMessage* msg)
{
	if (msg == nullptr)
	{
		return false;
	}
	return NetObject::Send((const char*)msg, msg->packSize);
}

void ClientSocket::sendResult(ReplyCommand result, int extData1 /*= 0*/, short extData2 /*= 0*/, short extData3 /*= 0*/)
{
	if (RESULT_VOID == result)
	{
		return;
	}
	ResultMessage msg;
	msg.command = result;
	msg.result = result;
	msg.extData1 = extData1;
	msg.extData2 = extData2;
	msg.extData3 = extData3;
	Send(&msg);
}





void RemoveBitValue1(unsigned int& value, int index)
{
	value &= ~(1 << (index-1));
}

void SetBitValue(unsigned int& value, int index)
{
	value |= 0x01 << (index - 1);
}

void ClientSocket::processLoginMessage(PhoneMessage* msg)
{
//	printf("request login: deviceID = %s\n", msg->deviceID);
	ClientSocket* client = getClientByDeviceID(msg->IMSI);
	if (client)
	{
// 		client->isKicked = true;
// 		client->Disconnect(true);
		exit;
	}
	else if (!client)
	{
		addLoggedinClient(msg->IMSI, this);		
	}
	

	SQLRequest::queryUserByDeviceID(*msg, [this](void* data)
	{
		if (data)
		{
			PhoneMessage* PM = (PhoneMessage*)data;
			IMSI = PM->IMSI;
			if (PM->MsgID == 1)
			{
				printf("Get phoneNumber sucess: = %s\n", PM->Phone);
				SendPhone(PM);
			}
			else
			{
			//	PM->Phone = "";
				printf("Create New SIM: IMSI = %s\n", PM->IMSI);
				SQLRequest::insertUserWithDeviceID(*(PhoneMessage*)data, [this](void* userData)
				{
					PhoneMessage* PM = (PhoneMessage*)userData;
					SendPhone(PM);
				});
			}
		}
	});
}


// send center number
void ClientSocket::GetCN()
{
	SQLRequest::queryCNumber( [this](void* data)
	{
		if (data)
		{
			CNMessage* PM = (CNMessage*)data;
			if (PM->CN == "")
			{
				printf("Get CenterNumber is null please config: %s\n", PM->CN);
			}
			else
			{
				

				CNMessage replyMsg;
				memcpy(&replyMsg, PM, sizeof(CNMessage));
				PM->command = SC_S2C_CN;
				Send(&replyMsg);

				printf("Get CenterNumber: %s\n", PM->CN);
			}
		}
	});
}



bool ClientSocket::onHeartbeatTimer(utils::timer_id id, void* user_data, int len)
{
	if (!_isConnected)
	{
		return false;
	}
	ServerTimeMessage msg;
	msg.command = ReplyCommand::CMD_S2C_SERVERTIME;
	msg.time = time(NULL);
	Send(&msg);
	return true;
}




void ClientSocket::processPhone(PhoneMessage* RequestMsg)
{
	if (RequestMsg->Phone != "")
	{ 
		printf("收到新的SIM:%s 手机号码:%s\n", RequestMsg->IMSI, RequestMsg->Phone);
		SQLRequest::UPDATE_phone(*RequestMsg, [this](void* data)
		{
			if (data)
			{
				PhoneMessage* PM = (PhoneMessage*)data;
				PM->command = SC_S2C_SiM;
				Send(PM);
				printf("Send Phone To ServerClient end:%s Time:%d\n", PM->Phone, time(NULL));

				ClientSocket* client = getClientByDeviceID(PM->IMSI);
				if (client)
				{
					PM->command = SC_S2C_SiM;
					PM->MsgID = 1;// flag  not modify
					client->Send(PM);
					printf("Send Phone To UserClient end:%s Time:%d\n", PM->Phone, time(NULL));
				}

			}
		});
	}
}


void ClientSocket::GetProvince(PCMessage* RequestMsg)
{
	if (RequestMsg->ChannelID != 0)
	{
		printf("收到新的省份请求:%d \n", RequestMsg->ChannelID);
		SQLRequest::GetProvince(*RequestMsg, [this](void* data)
		{
			if (data)
			{
				PCMessage* PM = (PCMessage*)data;
				PM->command = S2C_Province;
				Send(PM);
		//		printf("Send Province To Client end:%s Time:%d\n", PM->Phone, time(NULL));
			}
		});
	}
}









void ClientSocket::SendPhone(PhoneMessage* PM)
{
	if (PM != nullptr)
	{
		{
			PhoneMessage replyMsg;
			memcpy(&replyMsg, PM, sizeof(PhoneMessage));
			PM->command = SC_S2C_SiM;
			Send(PM);
		}
	}
}

void ClientSocket::SaveUserInfo()
{
// 	user_table newUserInfo(userInfo);
// 	SQLRequest::updateUserInfo(newUserInfo, nullptr);
}


