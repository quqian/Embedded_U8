#ifndef __UPGRADE_H__
#define __UPGRADE_H__


#include <stdint.h>
#include "SysFlash.h"

enum{
	FW_U8 = 0,
	FW_U8_BAK,
	DW_NUM,
};

#pragma pack(1)

typedef struct{
	uint16_t headFlag;            //0x55AA
    struct{
        uint8_t  upgradeFlag;       //0-没有升级信息 1-有升级信息
        uint8_t  fwVer;             //固件版本
        uint16_t checkSum;          //校验和
        uint32_t startAddrs;        //固件存储的偏移地址
        uint32_t fsize;             //固件大小
    }fw[DW_NUM]; 
}SYS_UPDATE_INFO_T;

#pragma pack()


int UpdateFromAppBkp(uint32_t startAddr,uint32_t fsize, uint16_t checksum);

#endif

