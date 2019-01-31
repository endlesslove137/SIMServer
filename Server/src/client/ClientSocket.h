#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

#include "Network/Network.h"
#include "database/Table.h"
#include "protocol/Message.h"
#include "protocol/Command.h"
#include "utils/timer.h"
#define AllBitTrue 0xFFFFFFFF

class ClientSocket :
	public NetObject
{
public:
	ClientSocket(void);
	virtual ~ClientSocket(void);

	virtual void OnAccept(unsigned int nNetworkIndex);
	virtual void OnDisconnect();
	virtual	unsigned int OnRecv(char *pMsg, unsigned int nSize);
	virtual void OnConnect(bool bSuccess);

	inline bool isConnected(){ return _isConnected; }
	inline unsigned int getIndex(){ return index; }

	std::string IMSI;
	user_table userInfo;
	user_table userInfo2;
	
private:
	bool BRank;
	unsigned int index;
	bool _isConnected;
	MessageHead* pHead;
	utils::timer_id heartbeatTimer;
	bool isKicked;

	bool Send(EmptyMessage* msg);
	void sendResult(ReplyCommand result, int extData1 = 0, short extData2 = 0, short extData3 = 0);

	bool onHeartbeatTimer(utils::timer_id id, void* user_data, int len);
	void processLoginMessage(PhoneMessage* msg);   //登陆消息*
	void processPhone(PhoneMessage* RequestMsg);   //电话号码
	void GetProvince(PCMessage* RequestMsg);      //获取省份配置
	void SendPhone(PhoneMessage* PM);
	void ClientSocket::GetCN();                   //获取中心机配置


	void SaveUserInfo();
	int RefreshRechargeReach(void* sumRecharge);

};
#endif
