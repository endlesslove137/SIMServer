#pragma once
#include "Database.h"
#include "Table.h"

const char* gb23122utf8(const char* gb2312);

class SQLRequest : public DBRequest
{
public:
	void OnRequest(Database& db) override;
	void OnFinish() override;

	static void queryUserByDeviceID(const PhoneMessage& info, const std::function<void(void* data)> &callback);
	static void insertUserWithDeviceID(const PhoneMessage& info, const std::function<void(void* data)> &callback);
	static void updateUserInfo(const user_table& info, const std::function<void(void* data)> &callback);
	static void updateDouble(const user_table& info, const std::function<void(void* data)> &callback);
	static void updateUserCharName(const user_table& info);
	static void GetVerion(const VersionMessage& info, const std::function<void(void* data)> &callback);
	static void GetBagInfo(const VersionMessage& info, const std::function<void(void* data)> &callback);
	static void Get4Net(const CharMessage& info, const std::function<void(void* data)> &callback);
	static void UPDATE_phone(const PhoneMessage& info, const std::function<void(void* data)> &callback);
	static void GetProvince(const PCMessage& info, const std::function<void(void* data)> &callback);
	static void GetRank(const std::function<void(void* data)> &callback);
	static void insertOrder(const OrderInfo& info, const std::function<void(void* data)> &callback);
	static void queryCNumber(const std::function<void(void* data)> &callback);


private:
	enum RequestType
	{
		QUERY_WITH_DEVICE_ID = 1,
		INSERT_WITH_DEVICE_ID,
		UPDATE_USER_INFO,
		QUERY_GetRank,
		UPDATE_USER_CharName,
		UPDATE_Double,
		Insert_Order,
		Query_Version,
		Query_BagInfo,
		Query_4Net,
		RT_UPDATE_phone,
		Get_Province,
		Get_CN
	};
	RequestType type;
	void* data;
	std::function<void(void* data)> callback;
};