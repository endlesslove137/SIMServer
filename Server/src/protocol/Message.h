#pragma once

#include <string>
#include <memory.h>
#pragma pack(push)
#pragma pack(1)

#define RankCount 10
#define CharNameLength 40



typedef enum
{
	RK_SumRechargeGet=1,
	RK_PackgeReach,
	RK_PackgeGet,
	RK_RoleReach,
	RK_RoleGet
}ReachKind;





struct RankInfo
{
	RankInfo() :hiscore(0), id(0)
	{
		memset(CharName, 0, sizeof(CharName));
	}
	unsigned int hiscore;
	unsigned int	id;
	char CharName[CharNameLength];
};

struct RankInfoList
{
	RankInfoList()
	{
		for (int i = 0; i < RankCount; i++)
		{
			RankInfoArray[i] = new RankInfo();
		}
	}
	~RankInfoList()
	{
		for (int i = 0; i < RankCount; i++)
		{
			delete RankInfoArray[i];
		}
	}
	RankInfo* RankInfoArray[RankCount];
};

typedef struct MessageHead
{
protected:
	~MessageHead(){};
	MessageHead(void) :command(0), packSize(0){}
	MessageHead(unsigned short size) :command(0), packSize(size){}

public:
	unsigned short command;
	unsigned short packSize;
} EmptyMessage; 

static_assert(sizeof(MessageHead)==4, "error");

struct LoginMessage : public EmptyMessage
{
	LoginMessage() :MessageHead(sizeof(LoginMessage))
	{
		memset(deviceID, 0, sizeof(deviceID));
		memset(macAddress, 0, sizeof(macAddress));
	}
	char			deviceID[64];
	char			macAddress[16];
};

struct PhoneMessage : public EmptyMessage
{
	PhoneMessage() :MessageHead(sizeof(PhoneMessage))
	{
		memset(IMSI, 0, sizeof(IMSI));
		memset(Phone, 0, sizeof(Phone));
	}
	short           MsgID;
	char			IMSI[17];
	char			Phone[17];
};

//CenterNumber
struct CNMessage : public EmptyMessage
{
	CNMessage() :MessageHead(sizeof(CNMessage))
	{
		memset(CN, 0, sizeof(CN));
	}
	char			CN[11];
};



struct PlayerInfoMessage : public EmptyMessage
{
	PlayerInfoMessage() :MessageHead(sizeof(PlayerInfoMessage)), userid(0), hiScore(0), rank(0), DoubleSec(0)
	, SumRechargeReach(0), SumRechargeGet(0), PackgeReach(0), PackgeGet(0), RoleReach(0), RoleGet(0), BFirstDouble(0)
	{

	}
	unsigned int	userid;
	unsigned int	hiScore;
	unsigned int	rank;
	unsigned int	DoubleSec;
	unsigned int	BFirstDouble;
	unsigned int	SumRechargeReach;
	unsigned int	SumRechargeGet;
	unsigned int	PackgeReach;
	unsigned int	PackgeGet;
	unsigned int	RoleReach;
	unsigned int	RoleGet;
};


struct RankInfoMessage : public EmptyMessage
{
	RankInfoMessage() :MessageHead(sizeof(RankInfoMessage))
	{
	}

	RankInfo RankInfoArray[RankCount];
};




struct HiscoreMessage : public EmptyMessage
{
	HiscoreMessage() :MessageHead(sizeof(HiscoreMessage))
	{

	}
	unsigned int		hiscore;
	unsigned long long	time;
};


struct CharNameMessage : public EmptyMessage
{
	CharNameMessage() :MessageHead(sizeof(CharNameMessage))
	{
		memset(CharName, 0, sizeof(CharName));

	}
	char		CharName[CharNameLength];
};


struct AddRechargeMessage : public EmptyMessage
{
	AddRechargeMessage() :MessageHead(sizeof(AddRechargeMessage)), Money(0), OrderResult(0), PayType(0)
	{
		memset(SPID, 0, sizeof(SPID));
		memset(OrderID, 0, sizeof(OrderID));

	}
	unsigned int	Money;
	char		OrderID[64];
	unsigned int	OrderResult;
	unsigned int	PayType;
	char		SPID[11];
};




struct ResultMessage : public EmptyMessage 
{
	ResultMessage() :MessageHead(sizeof(ResultMessage)), result(0), extData1(0), extData2(0), extData3(0)
	{

	}
	unsigned int	result;		//�����
	int				extData1;	//�������1
	short			extData2;	//�������2
	short			extData3;	//�������3
};


struct ServerTimeMessage : public EmptyMessage
{
	ServerTimeMessage() :MessageHead(sizeof(ServerTimeMessage)), time(0)
	{

	}
	unsigned long long	time;
};

struct BillMessage : public EmptyMessage
{
	BillMessage() :MessageHead(sizeof(BillMessage)), orderID(0)
	{
		memset(paycode, 0, sizeof(paycode));
	}
	char				paycode[16];
	unsigned long long	orderID;
};


struct ReachGetMessage : public EmptyMessage
{
	ReachGetMessage() :MessageHead(sizeof(ResultMessage)), kind(0), Bittag(0)
	{
	}
	unsigned char kind;
	unsigned char Bittag;
};


struct VersionMessage : public EmptyMessage
{
	char Sp[11];
	unsigned int Vsrsion;
	char Url[111];
	VersionMessage() :MessageHead(sizeof(VersionMessage)), Vsrsion(0)
	{
		memset(Url, 0, sizeof(Url));
		memset(Sp, 0, sizeof(Sp));
	}
};

struct VersionMessage2 : public EmptyMessage
{
	char Sp[11];
	unsigned int Vsrsion;
	char Url[111];
	unsigned int Baginfo;
	VersionMessage2() :MessageHead(sizeof(VersionMessage2)), Vsrsion(0), Baginfo(0)
	{
		memset(Url, 0, sizeof(Url));
		memset(Sp, 0, sizeof(Sp));
	}
};

struct CharMessage : public EmptyMessage
{
	char CM[31];
	CharMessage() :MessageHead(sizeof(VersionMessage))
	{
		memset(CM, 0, sizeof(CM));
	}
};

struct INTMessage : public EmptyMessage
{
	unsigned short Value;
	INTMessage() :MessageHead(sizeof(INTMessage))
	{
	}
};

struct PCMessage : public EmptyMessage
{
	unsigned short ChannelID;
	unsigned short Flag;
	PCMessage() :MessageHead(sizeof(PCMessage))
	{
	}
	PCMessage(unsigned short Value) :MessageHead(sizeof(PCMessage))
	{
		ChannelID = Value;
	}
};


#pragma pack(pop)
