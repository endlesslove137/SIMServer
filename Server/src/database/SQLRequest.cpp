#include "SQLRequest.h"
#include "utils/date.h"
#include "protocol/Message.h"


extern DBRequestQueue g_DBQueue;


const char* gb23122utf8(const char* gb2312)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return gb2312;
#else
	int len = MultiByteToWideChar(0, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(0, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(65001, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(65001, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
#endif 
}




void SQLRequest::OnRequest(Database& db)
{
	int iTemp(0);
	char sqlQuery[300] = { 0 };
	switch (type)
	{
	case RequestType::QUERY_WITH_DEVICE_ID:
	{
		PhoneMessage* PM = (PhoneMessage*)data;
		//		memset(PM->Phone, 0, sizeof(PM->Phone));
		unsigned int temp;
		std::string phone;
		//查询用户数据
		sprintf(sqlQuery, "SELECT Phone FROM phonenumber WHERE Phone !='' and IMSI='%s';", PM->IMSI);
		RecordsetPtr recordset = db.Query(sqlQuery);
		if (recordset.get() && recordset->MoveNext())
		{
			PM->MsgID = 1;
			Recordset& record = (*recordset);
			record >> phone;
			strcpy(PM->Phone, phone.c_str());
		}
		else
		{
			PM->MsgID = 0;
			sprintf(sqlQuery, "SELECT Phone FROM phonenumber WHERE IMSI='root';", "");
			RecordsetPtr recordset = db.Query(sqlQuery);
			if (recordset.get() && recordset->MoveNext())
			{
				Recordset& record = (*recordset);
				record >> phone;
				strcpy(PM->Phone, phone.c_str());
			}
		}
		break;
	}
	case RequestType::Get_CN:
	{
		CNMessage* PM = (CNMessage*)data;
		//		memset(PM->Phone, 0, sizeof(PM->Phone));
		unsigned int temp;
		std::string phone;
		//查询用户数据
		sprintf(sqlQuery, "SELECT Phone FROM phonenumber WHERE IMSI='root';","");
		RecordsetPtr recordset = db.Query(sqlQuery);
		if (recordset.get() && recordset->MoveNext())
		{
			Recordset& record = (*recordset);
			record >> phone;
			strcpy(PM->CN, phone.c_str());
		}
		else
		{
			memset(PM->CN, 0, sizeof(PM->CN));
		}
		break;
	}		
		case RequestType::INSERT_WITH_DEVICE_ID:
		{
			PhoneMessage* PM = (PhoneMessage*)data;
			//插入一条新数据
			sprintf(sqlQuery, "INSERT INTO phonenumber(ID,IMSI,UpdateTime) VALUES(NULL, '%s', NOW());", PM->IMSI);
			db.Query(sqlQuery);
			if (db.GetAffectedRows() == 1)
			{
// 				user->id = (unsigned int)db.GetInsertId();
// 				user->hiscore = 0;
// 				user->SP = "";
// 				user->createTime = utils::timeToString(time(NULL));
				//获取排行
			}
			else
			{
				delete data;
				data = nullptr;
			//	WRITE_FATAL_LOG("insert t_user error, deviceID=%s", user->deviceID.c_str());
			}
			break;
		}
		case RequestType::RT_UPDATE_phone:
		{
			 PhoneMessage* pm = (PhoneMessage*)data;
			 sprintf(sqlQuery, "UPDATE phonenumber SET Phone='%s' WHERE IMSI='%s'", pm->Phone, pm->IMSI);

			 db.Query(sqlQuery);
			 if (db.GetAffectedRows() != 0)
			 {
				// printf("%s \n", sqlQuery);
			 }
			 else
			 {
				// pm->MsgID = 0;
				 delete data;
				 data = nullptr;
			 }
			 break;
		}
		case RequestType::Get_Province:
		{
			PCMessage* PM = (PCMessage*)data;
			sprintf(sqlQuery, "SELECT ChannelID,Flag from provinceconf where ID=%d", PM->ChannelID);
			RecordsetPtr recordset = db.Query(sqlQuery);
			if (recordset.get() && recordset->MoveNext())
			{
				Recordset& record = (*recordset);
				record >> PM->ChannelID;
				record >> PM->Flag;
			}
			else
			{
				delete data;
				data = nullptr;
			}
			break;
		}

			
			
	}
}

void SQLRequest::OnFinish()
{
	if (callback)
	{
		callback(data);
	}
	if (data) 
	{
		delete data;
		data = NULL;
	}
}

void SQLRequest::queryUserByDeviceID(const PhoneMessage& info, const std::function<void(void* data)> &callback)
{
	auto request = new SQLRequest;
	request->type = RequestType::QUERY_WITH_DEVICE_ID;
	request->callback = callback;
	request->data = new PhoneMessage;
	memcpy(request->data, &info, sizeof(PhoneMessage));
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}

void SQLRequest::queryCNumber(const std::function<void(void* data)> &callback)
{
	auto request = new SQLRequest;
	request->type = RequestType::Get_CN;
	request->callback = callback;
	request->data = new CNMessage;
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}

void SQLRequest::insertUserWithDeviceID(const PhoneMessage& info, const std::function<void(void* data)> &callback)
{
	auto request = new SQLRequest;
	request->type = RequestType::INSERT_WITH_DEVICE_ID;
	request->callback = callback;
	request->data = new PhoneMessage;
	memcpy(request->data, &info, sizeof(PhoneMessage));
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}



void SQLRequest::updateUserInfo(const user_table& info, const std::function<void(void* data)> &callback)
{
	auto request = new SQLRequest;
	request->type = RequestType::UPDATE_USER_INFO;
	request->callback = callback;
	request->data = new user_table(info);
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}


void SQLRequest::updateDouble(const user_table& info, const std::function<void(void* data)> &callback)
{
	auto request = new SQLRequest;
	request->type = RequestType::UPDATE_Double;
	request->callback = callback;
	request->data = new user_table(info);
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}

void SQLRequest::GetRank(const std::function<void(void* data)> &callback)
{

	auto request = new SQLRequest;
	request->type = RequestType::QUERY_GetRank;
	request->callback = callback;
	request->data = new RankInfoList();
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));

}

void SQLRequest::updateUserCharName(const user_table& info)
{
	auto request = new SQLRequest;
	request->type = RequestType::UPDATE_USER_CharName;
	//request->callback = callback;
	request->data = new user_table(info);
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}

void SQLRequest::GetVerion(const VersionMessage& info, const std::function<void(void* data)> &callback)
{
	auto request = new SQLRequest;
	request->type = RequestType::Query_Version;
	request->callback = callback;
	request->data = new VersionMessage();
	memcpy(request->data, &info, sizeof(VersionMessage));
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}

void SQLRequest::GetBagInfo(const VersionMessage& info, const std::function<void(void* data)> &callback)
{
	auto request = new SQLRequest;
	request->type = RequestType::Query_BagInfo;
	request->callback = callback;
	request->data = new VersionMessage2();
	memcpy(request->data, &info, sizeof(VersionMessage));
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}

void SQLRequest::Get4Net(const CharMessage& info, const std::function<void(void* data)> &callback)
{
	auto request = new SQLRequest;
	request->type = RequestType::Query_4Net;
	request->callback = callback;
	request->data = new CharMessage();
	memcpy(request->data, &info, sizeof(CharMessage));
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}

void SQLRequest::UPDATE_phone(const PhoneMessage& info, const std::function<void(void* data)> &callback)
{
	auto request = new SQLRequest;
	request->type = RequestType::RT_UPDATE_phone;
	request->callback = callback;
	request->data = new PhoneMessage();
	memcpy(request->data, &info, sizeof(PhoneMessage));
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}

void SQLRequest::GetProvince(const PCMessage& info, const std::function<void(void* data)> &callback)
{
	auto request = new SQLRequest;
	request->type = RequestType::Get_Province;
	request->callback = callback;
	request->data = new PCMessage();
	memcpy(request->data, &info, sizeof(PCMessage));
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}


void SQLRequest::insertOrder(const OrderInfo& info, const std::function<void(void* data)> &callback)
{
	auto request = new SQLRequest;
	request->type = RequestType::Insert_Order;
	request->callback = callback;
	request->data = new OrderInfo(info);
	g_DBQueue.AddQueueMsg(std::shared_ptr<SQLRequest>(request));
}
