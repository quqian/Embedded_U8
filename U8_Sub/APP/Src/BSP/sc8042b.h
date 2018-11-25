#ifndef __SC8042B_H__
#define __SC8042B_H__

#include "includes.h"


#define AU_POWER_EN()   			gpio_bit_set(GPIOB, GPIO_PIN_0)
#define AU_POWER_DIS()  			gpio_bit_reset(GPIOB, GPIO_PIN_0)

#define AU_DATA_HIHG()  			gpio_bit_set(GPIOB, GPIO_PIN_4)
#define AU_DATA_LOW()   			gpio_bit_reset(GPIOB, GPIO_PIN_4)

#define AU_RST_EN()     			gpio_bit_set(GPIOB, GPIO_PIN_3)
#define AU_RST_DIS()    			gpio_bit_reset(GPIOB, GPIO_PIN_3)

#define READ_AU_BUSY()  			gpio_input_bit_get(GPIOB, GPIO_PIN_5)


#define Sc8042bSpeech   		PlayVoice


typedef enum{
	VOIC_NULL1=1,							//空
	VOIC_WELCOME=2,							//欢迎使用
	VOIC_NULL2=3,							//空
	VOIC_SHARE_CHARGE=4,					//共享充电
	VOIC_START_UPGRADE=5,                   //开始升级
	VOIC_READING_CARD=6,               		//正在读卡
	VOIC_DEVICE_EXCEPTION=7,				//设备故障
	VOIC_CARD_BANLANCE=8,					//卡片余额
	VOIC_CARD_RECHARGER=9,					//卡余额不足,请充值
	VOIC_CARD_INVALID=10,					//卡片无效
	VOIC_CARD_CHARGING=11,					//你的爱车正在充电
	VOIC_INPUT_SOCKET_NUM=12,				//请输入插座编号,按确认键
	VOIC_INPUT_CHARGRE_MONEY=13,			//请输入充电金额,按确认键.如需返回上一级,请按返回键
	VOIC_SOCKET_OCCUPID=14,					//插座被占用,请选用其他插座
	VOIC_SOCKET_ERROR=15,					//插座故障,请选用其他插座
	VOIC_SOCKET_NUM_INVALID=16,				//插座编号无效,请重新输入
	VOIC_NIN_YI_XUANZE=17,					//您已选择
	VOIC_HAO_CHAZUO=18,						//号插座
	VOIC_CHARGER_MONEY=19,					//充电金额
	VOIC_YUAN=20,							//元
	VOIC_VERIFIED_PLUG=21,					//请确认充电插座和车端插座均已插好
	VOIC_CARD_BANLANCE_INSUFFICIENT=22,		//余额不足,请重新输入
	VOIC_START_CHARGING=23,					//开始充电
	VOIC_STOP_CHARGER_TIP=24,				//如需结束充电,可拔掉插头
	VOIC_PLUG_IN_PLUG=25,					//请将插头插入
	VOIC_CARD=26,                          	//嘀（刷卡音）
	VOIC_KEY=27,                           	//噔（左右键按键音）
	VOIC_BLUETOOTH_ONLINE=28,				//蓝牙已连接
	VOIC_BLUETOOTH_OFFLINE=29,				//蓝牙已断开
	VOIC_ERROR=30,							//错误
	VOIC_0=31,                             	//0 (数字0)
    VOIC_1=32,                             	//1.
    VOIC_2=33,                             	//2.
    VOIC_3=34,                             	//3.
    VOIC_4=35,                             	//4.
    VOIC_5=36,                             	//5.
    VOIC_6=37,                             	//6.
    VOIC_7=38,                            	//7.
    VOIC_8=39,                             	//8.
    VOIC_9=40,                             	//9
	VOIC_TEST_TIP=41,						//1输桩号,2输识别码,3继电器测试
	VOIC_INPUT_SOCKET_TEST=42,				//输插座号按确定测试
	VOIC_HUNDRED=43,						//百
	VOIC_TEN=44,							//十
	VOIC_YUAN1=45,							//元
	VOIC_POINT=46,							//点
	VOIC_DEVICE_REBOOT=47,					//设备重启
	VOIC_SUCCESS=48,						//成功
	VOIC_A=49,								//a
	VOIC_B=50,								//b
	VOIC_C=51,								//c
	VOIC_D=52,								//d
	VOIC_E=53,								//e
	VOIC_F=54,								//f
	VOIC_TEM=55,							//温度
	VOIC_DEGREE=56,							//度
	VOIC_THOUSAND=57,						//千
	VOIC_START_CHARGER_FAIL=58,				//开启充电失败
	VOIC_POWER_TOO_LARGE=59,                //设备功率过大，无法充电
	VOIC_PLEASE_RECARD=60,                	//请再次刷卡以开启充电
	VOICE_NUM,
}SC8042B_VOICE_TYPE;



enum
{    
    ERROR_CODE_VOLATAGE_HIGH    = 21,       //设备计量过压
    ERROR_CODE_VOLATAGE_LOW     = 22,       //设备欠压
    ERROR_CODE_CURRENT_HIGH     = 23,       //设备过流
    ERROR_CODE_EMU_CONNECT      = 24,       //计量芯片通讯故障
    ERROR_CODE_TEMP_HIGH        = 25,       //设备温度过高
    ERROR_CODE_EEPROM           = 26,       //EEPROM故障
    ERROR_CODE_FLASH            = 27,       //Flash读写错误
    ERROR_CODE_GROUND           = 28,       //设备接地错误码
    
    ERROR_CODE_CARD_HEART       = 40,       //长时间未收到按键板的心跳包
    
    ERROR_CODE_2G_SEND_LENGTH   = 50,       //设备GPRS发送数据长度的时候错误
    ERROR_CODE_2G_SEND_FAILED   = 51,       //GPRS发送数据失败
    ERROR_CODE_OTA_FILE_SIZE    = 52,       //OTA升级文件大小错误
    ERROR_CODE_OTA_FILE_ERROR   = 53,       //OTA升级获取文件错误，或者网络异常
    ERROR_CODE_OTA_GET_FAILED   = 54,       //OTA升级Get数据失败
    ERROR_CODE_OTA_CHECK_FAILED = 55,       //FTP升级接受完数据后，计算校验和失败
    ERROR_CODE_OTA_FILE_SIZE_ERROR = 56,    //FTP升级接收完数据后计算数据大小失败
    ERROR_CODE_OTA_GET_FWCNT    = 57,       //FTP升级Get数据的第一帧16个字节数据失败,获取固件个数失败
    ERROR_CODE_OTA_GET_FWHEAD   = 58,       //FTP升级Get数据的第一帧16个字节数据数据头错误,固件头错误
    ERROR_CODE_OTA_GET_FW_NAME  = 59,       //FTP升级Get数据的第二帧16字节数据判断固件名字失败
    ERROR_CODE_OTA_FAILED       = 60,       //FTP升级失败
    ERROR_CODE_OTA_FLASH_CHECKSUM = 61,     //FTP升级,写入flash和读出的检验和错误
    ERROR_CODE_OTA_FLASH        = 62,        //FTP写入固件到flash错误
    
    ERROR_CODE_REGISTER         = 100,      //接收到服务器回复的注册消息，注册失败
    ERROR_CODE_LOGIN            = 101,      //接收到服务器回复消息，登录失败
    ERROR_CODE_START_GUN_NUM    = 102,      //检测到数据中的枪头号不在当前设备的枪头范围内
    ERROR_CODE_START_GUN_CGARGING= 103,     //检测当前枪处于充电状态
    ERROR_CODE_START_NO_COST    = 104,      //检测当前设备没有计费模板
    ERROR_CODE_STOP_GUN_NUM     = 106,      //接收服务器的远程关闭指令时候，数据中枪头号不在当前设备的枪头范围内
    ERROR_CODE_STOP_ACK_GUN_NUM = 107,      //发送完结束充电通知指令，接收服务器的ACK，数据中的枪的端口不在1——12之间
    ERROR_CODE_MESSAGE_UNKNONE  = 108,      //接收到服务器的消息，但是消息命令未知
    ERROR_CODE_MESSAGE_IDOCDE   = 109,      //接收到服务器的消息，设备注册时服务器回复消息中的桩号不是设备的桩号或者设备的识别码错误
    ERROR_CODE_MESSAGE_TYPE     = 110,      //接收到服务器的消息，消息的类型不是设备的消息
    ERROR_CODE_HEART_BEAT       = 111,      //长时间未收到服务器的心跳回复
    ERROR_CODE_HEART_BEAT_GUN   = 112,      //包装发送心跳数据，检测枪头状态错误
    ERROR_CODE_SOCKET           = 120,      //查询当前socket的连接状态，socket连接异常
	ERROR_CODE_REALY			= 121,		//继电器故障
};

extern void SC8042B_Init(void);
extern void OptFailNotice(int code);
extern void OptSuccessNotice(int code);
extern void TempNotice(uint16_t temp);
extern void SpeechChargeing(uint8_t id);
extern int PlayVoice(unsigned char cnt);
extern void SpeechCardBalance(uint32_t balance);


#endif


