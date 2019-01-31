#pragma once

enum RequestCommand
{
	SC_C2S_LOGIN = 1,
	SC_C2S_SiM =2,
	C2S_Province=3,
	C2S_getCN=4,
};

enum ReplyCommand
{
	CMD_S2C_SERVERTIME=9899,
	RESULT_VOID=9900,
	SC_S2C_LOGIN =9901,
	SC_S2C_SiM=9902,
	S2C_Province=9903,
	SC_S2C_CN = 9904	    //获取中心机号码
};