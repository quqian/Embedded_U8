#ifndef __APP_NETPROTO_H__
#define __APP_NETPROTO_H__


#include "includes.h"

#define OUT_NET_PKT_LEN					256
#define CHARGER_SN_LEN			        8   //bcd的长度
#define ORDER_SECTION_LEN				10

enum {
	MESSAGE_VER_NOENCRYPT = 1,
	MESSAGE_VER_ENCRYPT = 2,
};


enum {
    CARD_AUTH = 0,
    CARD_CHECK_MONEY,
	CARD_MONTH,
    CARD_IN_DOOR,
    CARD_OUT_DOOR,
};

enum {
    EVENT_PROMPT = 1,
    EVENT_ALARM,
    EVENT_FAULT,
};


enum {
    MQTT_CMD_REGISTER 		= 0,				//设备注册
	MQTT_CMD_START_UP 		= 1,                //登陆
	MQTT_CMD_CARD_ID_REQ 	= 2,               	//卡鉴权
	MQTT_CMD_HEART_BEAT 	= 7,                //心跳
	MQTT_CMD_DFU_DOWN_FW_INFO = 0x0A,       	//固件下载
	MQTT_CMD_REMOTE_CTRL 	= 0x0B,            	//远程控制
    MQTT_CMD_EVENT_NOTICE 	= 0x0D,           	//事件通知
	MQTT_CMD_DOWN_BAND_DEV	= 0x10,				//下发设备绑定
	MQTT_CMD_REQ_BAND_DEV	= 0x11,				//请求设备绑定
	MQTT_CMD_BAND_DEV_NOTIF	= 0x16,				//设备绑定完成通知
	MQTT_CMD_AES_REQ		=0x12,				//设备请求秘钥
	MQTT_CMD_UPDATE_AES_NOTICE=0x13,			//后台更新秘钥通知
};


enum {
	EVENT_START             = 1,                    //1.
	EVENT_SHUT_DOWN         = 2,                    //2.
	EVENT_POWER_DROP        = 3,                    //3.
	EVENT_NET_BREAK         = 4,                    //4.
	EVENT_NET_RECONNECT     = 5,                    //5.
	EVENT_CARD_FAULT        = 0x0A,                 //0x0A
    EVENT_ENV_TEMP_HIGH     = 0x0E,                 //0x0E
    EVENT_CHIP_TEMP_HIGH    = 0x0F,                 //0x0F
    EVENT_CHIP_FAULT        = 0x12,                 //0x12 //器件故障 
    EVENT_SMOKE_WARING      = 0x15,                 //烟感报警
    EVENT_DOOR_OPEN_WARING  = 0x16,                 //门匝强开
};

/**
 *	远程控制
 * 1：系统立即重启 2：开启枪头 3：关闭枪头 4：进入维护状态，关闭充电服务 5：开启充电服务 6：设定最大输出功率
 * 7: 设定充满功率阀值	8:设定充满时间阀值	9: 设定插枪时间阀值		10: 拔枪是否停止充电订单	11: 设置刷卡板工作模式
 */
enum {
    SYSTEM_REBOOT = 1,
    CTRL_OPEN_GUN,                                  //0x02
    CTRL_CLOSE_GUN,                                 //0x03
    CTRL_SET_SERVER_STATUS,                         //0x04
    CTRL_SET_OUT_POWER,                             //0x05
    CTRL_SET_FULL_POWER,                            //0x06
    CTRL_SET_FULL_TIME,                             //0x07
    CTRL_SET_PULL_GUN_TIME,                         //0x08
    CTRL_SET_PULL_GUN_STOP,                         //0x09
    CTRL_SET_CARD_WORK,                             //0x0a
    CTRL_CLEAR_DATA,                                //0x0b
    CTRL_SET_PRINT_SWITCH,                          //0x0c
	CTRL_SET_DISTURBING_TIME,						//0x0D
	CTRL_SET_CHARGING_FULL_STOP = 0x11,             //0x11
	CTRL_OPEN_DOOR				= 0x12,
};

//*********************************************************/


typedef struct 
{
	uint8_t flag ;			//1-发送送  0--无发送
	uint8_t direction;	
	uint8_t cardId[16];
	uint16_t sn;
	uint8_t repeat;
	uint32_t startTime;
}CardAuthorRepeatMsg;



#pragma pack(1)

/**
 *
 */
typedef struct {
    uint8_t  aa;                                //0.
    uint8_t  five;                              //1.
    uint8_t  type;                              //2：x9 2：x10
    uint8_t  chargerSn[CHARGER_SN_LEN];         //3.
    uint16_t len;                               //4. 包含从 版本域 到 校验和域 的所有字节数
    uint8_t  ver;                               //5. b0:为1就是不支持加密;0支持加密；b1:本报文是否加密；b2~3:加密算法类型0:AES ECB
    uint16_t sn;                                //6.
    uint8_t  cmd;                               //7.
}PKT_HEAD_STR;

typedef struct {
    PKT_HEAD_STR head;
    uint8_t  data[OUT_NET_PKT_LEN];
}FRAME_STR;


/**
 *
 */
typedef struct {
    uint8_t  device_type[20];                   //1.设备类型
    uint8_t  register_code[16];                 //2.注册码
	char	 HWId[20];							//硬件id
}REGISTER_REQ_STR;

typedef struct {
    uint8_t  result;
    uint8_t  idcode[8];                         //中心平台分配的设备识别码 bcd
}REGISTER_ACK_STR;


/**
 *
 */
typedef struct {
	uint8_t  device_type[20];                   //1.
    uint8_t  chargerSn[CHARGER_SN_LEN];         //2.桩编号 bcd
	uint8_t  fw_version;                        //3. x10版本号
	uint8_t  fw_1_ver;                          //4. X10按键板版本号
    uint8_t  sim_iccid[20];                     //5.
    uint8_t  onNetWay;                          //6.上网方式  1：本地上网；2：485路由上网；3：2.4G路由上网 4:wifi上网
    uint8_t  modeType;                          //7. 1：2G；2：4G；3：nbIot-I；4：nbIot-II
	uint8_t  login_reason;						//8. 1:上电启动			2:离线恢复重发
	uint8_t  gun_number;                        //9. 充电桩可用充电口数量，最大128个端口
	uint8_t  device_status;                     //10. 指示设备离线时曾经出现过的错误状态
	uint8_t  statistics_info[8];				//11. 统计信息 [0~2]:拨号错误 [3]续冲时长,分钟 [4]拔枪/充满自停参数 [5-6]禁音时间 [7]按键板固件版本
    uint8_t  downloadType;                      //12 0-ftp 1-http
}START_UP_REQ_STR;

typedef struct {
    uint8_t  result;							//1.登录结果
	uint32_t time_utc;							//2.服务器时间
}START_UP_ACK_STR;



/**
 *
 */
typedef struct {
	uint8_t  gun_id;                            //0.
	uint8_t  card_type;                         //1.
    uint8_t  optType;                           //2： 0：刷卡鉴权，开始充电；1：仅查询余额；2：手机用户反向鉴权 3：刷卡进车棚 4：刷卡出车棚
	uint8_t  card_id[16];                       //3.
	uint8_t  card_psw[8];                       //4.
    uint8_t  mode;                              //5.充电模式  0：智能充满 1：按金额 2：按时长 3：按电量
	uint32_t chargingPara;                      //6.充电参数  智能充满，为0  按金额，分辨率1分  按时长，分辨率1分钟  按电量，分辨率0.01kwh
}CARD_AUTH_REQ_STR;

typedef struct {
    uint8_t  gun_id;                            //0.
	uint8_t  result;                            //1.鉴权结果
	uint8_t  cardType;                          //2. 1月卡
	uint8_t  rsv;								//3
	uint32_t user_momey;                        //4.账户余额 分
	uint8_t  order[ORDER_SECTION_LEN];          //5.订单号
}CARD_AUTH_ACK_STR;



/**
 *
 */
typedef struct {
    uint8_t  netSigle;                          //1. Sim卡信号
	uint8_t  envTemp;                           //2. 环境温度 度 -50度偏移  -50~200
	uint8_t  KberrCnt;
	uint8_t  doorState;
	uint8_t	 rev[2];
    uint8_t  gunCnt;                            // U8项目为0
}HEART_BEAT_REQ_STR;

typedef struct {
    uint32_t time;
    uint8_t  status;                            //0，	接收成功   1，	接收失败
}HEART_BEAT_ACK_STR;


/**
 * 固件升级
 */
typedef struct{
	char     url[48];                           //升级服务器地址，不足补0
	char     usrName[4];                        //登录用户名
	char     psw[4];                            //密码
    char     fileName[8];                       //文件名
	uint32_t checkSum;                          //文件字节累加和
    uint32_t httpLen;               
    char     httpUrl[128];
}DOWN_FW_REQ_STR;

typedef struct {
	uint8_t result;                             //0: 升级成功  1: 接收失败 2: 校验失败 3: 写入失败
}DOWN_FW_ACK_STR;


/**
 * 远程控制
 */
typedef struct {
    uint8_t  optCode;                           //控制代码
    uint32_t para;                              //控制参数
}REMO_CTRL_REQ_STR;

typedef struct {
    uint8_t  optCode;
    uint8_t  result;
}REMO_CTRL_ACK_STR;



/**
 * 事件通知
 */
typedef struct {
    uint8_t  gun_id;                            //0. 如果是0表示整桩,1~128,插座接口
    uint8_t  code;                              //1. 事件代码
    uint8_t  para1;                             //2.
    uint32_t para2;                             //3.
    uint8_t  status;                            //4.1：产生  2：恢复
    uint8_t  level;                             //5.事件等级 1提示  2告警  3严重故障
    char     discrip[32];          				//6.事件详细描述
}EVENT_NOTICE_STR;

typedef struct {
    uint8_t  result;                            //0、接收成功；1、接收失败
    uint8_t  gun_id;                            //0. 如果是0表示整桩,1~128,插座接口
    uint8_t  code;                              //1. 事件代码
    uint8_t  para1;                             //2.
    uint32_t para2;                             //3.
    uint8_t  status;                            //4.1：产生  2：恢复
    uint8_t  level;                             //5.事件等级 1提示  2告警  3严重故障
}EVENT_NOTICE_ACK_STR;



/**
 * 设备绑定
 */
typedef struct{
	uint8_t subDeviceNum;
	uint8_t subDeviceAdd[RF_DEV_MAX][5];
}BIND_DEVICE_STR;

typedef struct{
	uint8_t result;
}BIND_DEVICE_ACK_STR;

typedef struct{
	uint8_t rev;
}BIND_DEVICE_REQ_STR;


/**
 * 设备绑定完成通知
 */

typedef struct{
	uint8_t result;
}BIND_DEVICE_NOTIF_ACK_STR;


#pragma pack()

uint16_t GetFrameStrSn(void);
int App_NetProto_SendRegister(void);
int App_NetProto_SendStartUpNotice(int flag);
int App_NetProto_SendCardAuthReq(uint8_t cardId[],int flag,uint16_t sn);
int App_NetProto_SendEventNotice(uint8_t gunId, uint8_t event, uint8_t para1, uint32_t para2, uint8_t status, char *pDisc);
int App_NetProto_SendHeartBeat(void);
void App_NetProto_SendRemoCtrlAck(FRAME_STR *pRemoCtrlReq, uint8_t result);
int App_NetProto_SendReqBindDev(void);
void App_NetProto_SendBindDeviceAck(FRAME_STR *pFrame,uint8_t result);
int App_NetProto_SendBindDevFinishedNotif(void);

extern CardAuthorRepeatMsg cardAuthorMsg;

#endif

