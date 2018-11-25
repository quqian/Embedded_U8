#ifndef __FLASH_USR_H__
#define __FLASH_USR_H__

#include "includes.h"


#pragma pack(1)
typedef struct{
	uint16_t updateFlag;
    uint8_t  fwVer;             //固件版本
	uint16_t checkSum;          //校验和
	uint32_t fsize;             //固件大小
    uint16_t segmentNum;        //分片的数目
}SYS_UPDATE_INFO_T;

typedef struct{
	uint32_t updateFlag;
    uint32_t size;
    uint16_t checkSum;
}SYS_UPDATE_INFO_STR;

#pragma pack()

extern SYS_UPDATE_INFO_T   UpdateInfo ;

extern int UpgradeFirmware(uint8_t *fwBuffer,uint32_t fwsize, uint32_t checkSum,uint16_t fwSn);
extern int FlashWriteSysInfo(void *pSysInfo, uint16_t size);
extern void FlashReadSysInfo(void *pSysInfo, uint16_t size);
extern int FlashTest(void);

#endif



