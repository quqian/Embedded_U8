#ifndef __APP_BLUEPROTO_H__
#define __APP_BLUEPROTO_H__

#include <stdint.h>


enum {
    B_SHAKE_REQ = 1,			//握手请求
    B_OPPO_SITE_AUTH = 2,		//用户鉴权
    B_START_CHARGING = 3,		//开启充电
    B_STOP_CHARGING = 4,		//结束充电
    B_HEART_BEAT = 5,			//心跳
    B_COST_TEMPLATE_UPLOAD = 6,	//计费模板上传
    B_REQ_COST_TEMPLATE = 7,	//请求计费模板
    B_FW_UPGRADE_NOTICE = 8,	//固件升级开始通知
    B_FW_DOWN_LOAD = 9,			//固件下发
    B_REQ_BREAK = 0x0A,			//请求断开蓝牙链接
	B_DEV_REGISTER	= 0x0B,		//设备注册
	B_DEV_LOGIN	= 0x0C,			//设备登陆
	B_HISTORY_ORDER_UPLOAD=0x0D,//历史订单上传
	B_COST_TEMPLATE_DOWNLOAD=0x0E,//计费模板下发
	B_SET_DEV_SERIALNUM = 0x0F,	//设置充电桩编号
	B_SET_DEV_CODER	= 0x10,		//设置充电桩识别码
	B_SET_GW_ADDR = 0x11,		//设置2.4G网关地址
	B_SET_SERVER_IP	 = 0x12,	//设置服务器IP
	B_SET_SERVER_PORT = 0x13,	//设置服务器端口
	B_SET_TERMINAL_NUM = 0x14,	//设置终端编号信息
	B_REMOTE_CTRL = 0x15,		//远程控制
	B_HISTORY_ORDER_ENSURE = 0x16,//历史订单上报确认
	B_UPLOAD_DEVICE_STATE = 0x17,	//
	B_SET_WIFI_INFO		= 0x18,		//设置WiFi信息
};


#pragma pack(1)

typedef struct {
    uint8_t  start;
    uint8_t  cmd;
    uint8_t  len;
}BLUE_PROTO_HEAD_STR;

typedef struct {
    BLUE_PROTO_HEAD_STR head;
    uint8_t  data[255];
}BLUE_PROTO_STR;


/***********手机握手请求**********/
typedef struct {
    uint32_t time;
	char phonesSn[12];
}BLUE_SHAKE_REQ_STR;

typedef struct {
    char     name[4];                               //0. 设备型号
    uint8_t  chargerSn[5];                          //1. 桩编号
    uint8_t  fwVer;                                 //2. 固件版本号
    uint8_t  portCnt;                               //3. 插座数目
    uint8_t  startNo;                               //4. 插座起始编号
}BLUE_SHAKE_ACK_STR;



/**********遥信及心跳*********/ 
typedef struct {
    uint8_t  simSignal;                             //0. Sim卡信号
    uint8_t  temp;                                  //1. 环境温度 度 -50度偏移  -50~200
    uint8_t  portCnt;                               //2. 本次报文所包含的充电接口数目 0
}HEART_BEAT_STR;

typedef struct {
    uint32_t time;                                  //0. 系统时间
    uint8_t  result;                                //1. 0接收成功；1接收失败
}BLUE_HEART_BEAT_ACK_STR;



/**********固件升级********/ 
typedef struct{
	uint32_t     fw_size;
	uint32_t     package_num;                       
	uint16_t     checkSum;
    uint8_t      fw_version;
}BLUE_DOWN_FW_REQ_STR;
typedef struct {
	uint8_t result;                             //0: 升级成功  1: 接收失败 2: 校验失败 3: 写入失败
}BLUE_DOWN_FW_ACK_STR;

typedef struct {
	uint8_t data[64];
}BLUE_FW_DOWNLOAD_REQ_STR;
typedef struct {
	uint8_t result;
	uint8_t index;
}BLUE_FW_DOWNLOAD_ACK_STR;


/**********请求断开链接********/ 
typedef struct {
	uint32_t     timestamp;	
}BLUE_DISCONNECT_DEV_REQ_STR;
typedef struct {
	uint8_t     status;	
}BLUE_DISCONNECT_DEV_ACK_STR;


/**********设置充电桩编号*********/
typedef struct {
	uint8_t  chargerSn[8];
}BLUE_SET_CHARGERSN_REQ_STR;
typedef struct {
	uint8_t  result;
}BLUE_SET_CHARGERSN_ACK_STR;

/**********设置充电桩识别码*********/
typedef struct {
	uint8_t idcode[8];
}BLUE_SET_CHARGERIDCODE_REQ_STR;
typedef struct {
	uint8_t  result;
}BLUE_SET_CHARGERIDCODE_ACK_STR;


/**********远程控制*********/
typedef struct {
	uint8_t  cmd;
	uint32_t pararm;
}BLUE_REMOTE_CTRL_REQ_STR;
typedef struct {
	uint8_t  cmd;
	uint32_t pararm;
	uint8_t result;
}BLUE_REMOTE_CTRL_ACK_STR;



typedef struct{
	char ssid[32];
	char passwd[32];
}BLUE_SET_WIFI_INFO_STR;

typedef struct{
	uint8_t result;
}BLUE_SET_WIFI_INFO_ACK;

#pragma pack()


void App_Blue_SendHeartBat(void);

#endif


