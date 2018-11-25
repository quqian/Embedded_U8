#ifndef __COM_PROTO_H__
#define __COM_PROTO_H__


#include "includes.h"



enum{
    TEST_CMD_KEY = 1,			//按键测试
	TEST_CMD_CARD = 2,			//刷卡测试
	TEST_CMD_LCD = 3,			//LCD测试
	TEST_CMD_2G = 4,			//2G模块测试
	TEST_CMD_VOICE = 5,			//喇叭测试
	TEST_CMD_RELAY = 6,			//继电器
	TEST_CMD_PCB_CODE = 7,			//PCB编码
    DEBUG_CMD_SN = 8,
    TEST_CMD_BLE = 0x09, //BLE测试
    TEST_CMD_FW_VERSION = 0x0a,
    TEST_CMD_READ_PCB = 0x0b,
    DEBUG_CMD_DEV_TYPE  = 0x0d,                     //0x0d
    DEBUG_CMD_GW_ADDR = 0x0e,                              //0x0e
    DEBUG_CMD_TER_MINAL = 0x0f,                            //0x0f
    DEBUG_CMD_SERVER_ADDR = 0x10,                          //0x10
    DEBUG_CMD_SERVER_PORT = 0x11,                          //0x11
    DEBUG_CMD_PRINTF,                               //0x12
    DEBUG_CMD_REBOOT,                               //0x13
    DEBUG_CMD_DEVICE_ID,                            //0x14
    DEBUG_CMD_SET_RTC,                          //0x15 设置RTC时钟
    DEBUG_CMD_RED_RTC,                          //0x16 读取RTC时钟
    DEBUG_CMD_FLASH,                            //0x17 Flash读写测试
	DEBUG_CMD_SET_LISTEN_MODE,                  	//0x18
    DEBUG_CMD_QUE_LISTEN_MODE,                  	//0x19
    DEBUG_CMD_LED,                              //0x1a Led指示灯测试
    DEBUG_CMD_485,                              //0x1b 485接口测试
    DEBUG_CMD_NET_CONNECT,                      //0x1c 后台对接测试
    DEBUG_CMD_4G_TO_2G,                         //0x1d 设置4G兼容2G
    DEBUG_CMD_GET_CHARGER_SN,                   //0x1e 获取桩编号
    DEBUG_CMD_GET_DEVICE_ID,                    //0x1f 获取识别码
    START_AGE_TEST,                             //0x20 启动老化测试
    GET_AGE_TEST_RESULT,                        //0x21 获取老化测试结果
    GET_CMD_RS232 = 0x28,                        //0x28 RS232
    DEBUG_CMD_24G_COMMUNICATION = 0xF3,                    //2.4G通信测试 
    TEST_CMD_STATUS = 0x99,
};


typedef enum {
    FIND_AA,
    FIND_55,
    FIND_CHARGER_TYPE,
    FIND_CHAGER_SN,
    FIND_SRC,
    FIND_DEST,
    FIND_LEN,
    FIND_VER,
    FIND_SERNUM,
    FIND_CMD,
    RX_DATA,
    FIND_CHK,
    FIND_END,
} PROTO_MSG_STR;
	

#pragma pack(1)

typedef struct {
    uint8_t  aa;                                    //0.
    uint8_t  five;                                  //1.
    uint8_t  src[5];                                //2. 桩号   Bcd
    uint8_t  dest[5];                               //3.
    uint16_t len;                                   //4.
    uint8_t  ver;                                   //5.
    uint8_t  sn;                                    //6.
    uint8_t  cmd;                                   //7.
}OUT_NET_HEAD_STR;

typedef struct {
    OUT_NET_HEAD_STR head;
    uint8_t  data[128];
}OUT_PKT_STR;


#pragma pack()



extern void DebugRecvProc(void);
extern void SendTestPktAck(uint8_t cmd, uint8_t *pkt, uint8_t len);

#endif






