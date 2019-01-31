
#ifndef __TABBLE_H__
#define __TABBLE_H__
#include <string>
#include "protocol/Message.h"




struct user_table{
	user_table() :id(0){
		IMSI = "";
		Phone = "";
		UpdateTime = "";
	}
	user_table(const user_table& rhs){
		id = rhs.id;
		IMSI = rhs.IMSI;
		Phone = rhs.Phone;
		UpdateTime = rhs.UpdateTime;

	}
	unsigned int	id;
	std::string		IMSI;			//�豸��(Ψһ)
	std::string		Phone;			//�������
	std::string		UpdateTime;			//�û�����ʱ�� 
//	std::string		lastLoginTime;		//����½ʱ��
};


struct OrderInfo{
	OrderInfo() :Money(0), OrderResult(0), PayType(0), UserID(0){
		OrderID = "";
		SPID = "";
	}
	OrderInfo(const user_table& rhs){
		UserID = rhs.id;
		Money = 0;
		OrderID = "";
		OrderResult = 0;
		PayType = 0;
		SPID = "";
	}
	OrderInfo(const OrderInfo& rhs){
		UserID = rhs.UserID;
		Money = rhs.Money;
		OrderID = rhs.OrderID;
		OrderResult = rhs.OrderResult;
		PayType = rhs.PayType;
		SPID = rhs.SPID;
	}
	OrderInfo(const AddRechargeMessage* msg){
		Money = msg->Money;
		OrderID = msg->OrderID;
		OrderResult = msg->OrderResult;
		PayType = msg->PayType;
		SPID = msg->SPID;
		UserID = 0;

	}


	unsigned int	UserID;
	unsigned int	Money;
	std::string		OrderID;
	unsigned int	OrderResult;
	unsigned int	PayType;
	std::string		SPID;
};


#endif
