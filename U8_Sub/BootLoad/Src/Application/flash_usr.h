#ifndef __FLASH_USR_H__
#define __FLASH_USR_H__

#include "includes.h"


#pragma pack(1)

typedef struct{
	uint32_t updateFlag;
    uint32_t size;
    uint16_t checkSum;
}SYS_UPDATE_INFO_STR;

#pragma pack()

extern SYS_UPDATE_INFO_STR updateInfo;



extern int UpdateFromAppBkp(uint32_t fsize, uint16_t checksum);

#endif



