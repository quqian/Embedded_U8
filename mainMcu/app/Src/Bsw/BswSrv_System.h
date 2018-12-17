#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "includes.h"

#define ICCID_LEN                       20
#define CHARGER_SN_LEN			        8

enum{
	NETTYPE_WIFI = 0,
	NETTYPE_GPRS = 1,
	NETTYPE_485,
	NETTYPE_24G,
};


enum{
	OTA_NONE  = 0,
	OTA_NET   = 1,
	OTA_BLUE  = 2,
	OTA_UART  = 3,
};

typedef struct{

	uint16_t bandSize;					//绑定个数
	struct
	{
		uint8_t num;			//编号
		uint32_t address;		//已经绑定的设备地址
	}Unit[RF_DEV_MAX];

}RFDev_t;

//sizeof(SYSTEM_INFO_T) = 256
typedef struct
{
	uint32_t magic_number;						//
    uint8_t stationId[8];                     	// 设备号bcd, SN=10, 10 byte  0001123456789
	uint8_t idCode[8];							// 注册码
	char WifiName[32];							// ssid
	char WifiPasswd[32];						// passwd
	uint8_t netStep;							//联网优先级 0-wifi  1-4G
	uint8_t pcbSn[8];							//PCB编号
	uint8_t reserved0[7];
	RFDev_t RfDev;								// 433设备 最大20个设备 占用102个字节
	uint8_t resevred1[54];						// 预留
}SYSTEM_INFO_T;


typedef struct
{
	uint8_t isRegister;						//0-未注册  1-已注册
	uint8_t isLogin;						//0-未登录	1-已登陆
	uint8_t card_state;                     //读卡器状态   0: 表示初始化出错  1 :表示初始化ok 
	uint8_t WG215InitOK;					//是否检测到WG215模块 0-未检测到 1-检测OK
	uint8_t BlueInitOk;						//蓝牙是否初始化 0：未初始化	1：已经初始化
	uint8_t CBInitOK;						//刷卡版是否初始化成功 0未初始化 1-初始化成功
	uint8_t netType;						//上网方式	1-本地2G/4G 2-485  3-2.4G 4-wifi
	uint8_t modeType;						//模块型号
	uint8_t gSimStatus;						//sim卡状态
	uint8_t simSignal;						//sim卡信号值
	uint8_t iccid[ICCID_LEN+1];				//sim卡的iccid
	uint8_t doorState;						//门禁状态 0--关闭  1--打开
	uint8_t doorStatePre;					
	uint8_t is_socket_0_ok;					//socket是否建立连接 0-未连接 1-已连接
	char BlueName[16];						//蓝牙名称
	uint8_t CBVerson;						//刷卡版版本
	uint32_t lastRecvCBTime;				//上次接收到刷卡版的心跳时间	
	uint8_t isBlueConnect;					//是否有蓝牙连接
	uint8_t isBlueLogin;					//蓝牙是否登陆
	char phonesSn[20];						//蓝牙连接手机号码
	uint32_t blueLastOnlineTime;			//蓝牙在线时间
	uint8_t upgradeFlag;					//0-未升级  非0正在升级 1-网络升级   2--蓝牙升级 3--串口升级
	uint32_t upgradeTime;					//升级时间
	uint8_t isTest;							//是否进入测试模式0-未进入  1-进入测试模式
	void (*readCard_Callback)(uint8_t,uint8_t *);
	uint8_t netStep;						//联网优先级 0-wifi  1-4G
}GLOBAL_INFO_T;


void BswSrv_LoadSystemInfo(void);
void BswSrv_SystemResetRecord(void);
int BswSrv_GetHWId(char HWId[]);
int BswSrv_GetCpuTemp(void);
void BswSrv_SystemReboot(void);
void BswSrv_RF433AddrToChar(uint32_t addr,uint8_t ch[]);
uint32_t BswSrv_CharToRF433Addr(uint8_t ch[]);

uint16_t BswSrv_Tool_CheckSum(uint8_t *data,uint16_t len);
int BswSrv_Tool_isArraryEmpty(uint8_t *array,int len);
int BswSrv_Tool_StringToBCD(unsigned char *BCD, const char *str) ;
char *BswSrv_Tool_BCDToString(char *dest, unsigned char *BCD, int bytes) ;

extern SYSTEM_INFO_T	SystemInfo;
extern GLOBAL_INFO_T	GlobalInfo;


#endif


