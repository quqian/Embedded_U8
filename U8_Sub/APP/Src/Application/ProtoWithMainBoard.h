#ifndef __PROTO_WITH_MAINBOARD_H__
#define __PROTO_WITH_MAINBOARD_H__


#include "includes.h"
#include "flash_usr.h"
#include "flash.h"

enum{
    ENUM_MODUL_BASE = 0x01,
	ENUM_MODUL_CARD = 0x02,
	ENUM_MODUL_UPGRADE = 0x03,
	
	ENUM_SHAKE_HAND = 0x01,
	ENUM_HEART_BEAT = 0x02,
	
	ENUM_READ_CARD = 0x01,
	ENUM_WRITE_CARD = 0x02,
	
	ENUM_UPGRADE_REQUEST = 0x01,
	ENUM_SEND_UPGRADE_PKT = 0x02,
};

typedef enum {
	CB_FIND_AA=0,
	CB_FIND_FIVE=1,
	CB_FIND_LEN=2,
	CB_FIND_VER=3,
	CB_FIND_SN=4,
	CB_FIND_MODULE=5,
	CB_FIND_CMD=6,
	CB_FIND_DATA=7,
	CB_FIND_CHECKSUM=8,
}CB_FIND;

#pragma pack(1)

typedef struct {
    uint8_t  aa;                                    //0.
    uint8_t  five;                                  //1.
	uint16_t len;								    //2. 长度
    uint8_t  ver;                                   //3. 版本号
    uint8_t  sn;                                    //4. 报文流水号
	uint8_t  module;							    //5. 01-基本操作   02-卡操作  03-远程升级
    uint8_t  cmd;                                   //6. 命令代码
}CB_HEAD_STR;

typedef struct {
    CB_HEAD_STR head;
    uint8_t  data[256 - sizeof(CB_HEAD_STR)];
}CB_STR_t;


typedef struct{
    uint8_t fw_verson;
    uint8_t cardState;					// 0—故障  1—正常
    uint32_t TimeStamp;	
}SHAKE_HAND_STR;

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}SHAKE_HAND_ACK_STR;

typedef struct{
    uint8_t cardState;					// 0—故障  1—正常
    uint32_t TimerStamp;				//发送给主板
}HEARTBAT_STR;

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
    uint32_t TimerStamp;				//主板时间戳，0-无效 (如果主板无网络，该位填0，如果主板有网络，该位填真实时间戳，刷卡版需要对时)
}HEARTBAT_ACK_STR;

typedef struct{
    uint8_t cardType;
    char cardId[16];
    uint8_t reseverd[4];
}READ_CARDINFO_STR;

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}READ_CARDINFO_ACK_STR;

typedef struct{
    uint8_t sector;						// 0~15
    uint8_t block;						// 相对扇区的偏移块号 0~3
    uint8_t data[16];
}WRITE_CARDINFO_STR;

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}WRITE_CARDINFO_ACK_STR;

typedef struct{
    uint32_t filesize;
    uint32_t package;
    uint16_t checkSum;
    uint8_t  fw_verson;
}START_UPGRADE_REQUEST_STR, *pSTART_UPGRADE_REQUEST_STR;

typedef struct{
    uint8_t result;						// 0: 可以升级；1：拒绝升级
}START_UPGRADE_REQUEST_ACK_STR;

typedef struct{
    uint8_t index;
    uint8_t  data[64];
}DOWN_LOAD_FW_STR,*pDOWN_LOAD_FW_STR;

typedef struct{
	uint8_t result;						// 0: 接收成功；1：失败
	uint8_t index;
}DOWN_LOAD_FW_STR_ACK;

#pragma pack()



extern void ComRecvMainBoardData(void);
extern int BasicInfoShakeHand(void);
extern int BasicInfoHeartBeat(void);
extern int SwipeCardReadCard(void);





#endif






