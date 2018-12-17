#ifndef __FLASH_USR_H__
#define __FLASH_USR_H__

#include "includes.h"
#include "BswDrv_Sys_Flash.h"


typedef enum{
	FIRMWARE_PART = 0   ,
	SYSCONF_PART        ,
	UPGRADEHEAD_PART    ,
    RESEVERD_PART       ,
	PART_NUM            ,
}PARTITION;

typedef struct {
    uint32_t s_base;  		//start addr
    uint16_t s_count; 		//section count
}FLASH_PART;



void FlashErase(PARTITION n);
int FlashWriteSysInfo(void *pSysInfo, uint16_t size);
void FlashReadSysInfo(void *pSysInfo, uint16_t size);


#endif
