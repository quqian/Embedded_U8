#ifndef __FLASH_H__
#define __FLASH_H__

#include "includes.h"


// void FlashWrite(uint32_t address,uint8_t *writeBuffer,uint16_t writeLen);


void BswDrv_SysFlashRead(uint32_t address,uint8_t *pBuffer,uint16_t readNum);
int BswDrv_SysFlashWrite(uint32_t address,uint8_t *writeBuffer,uint16_t writeLen);
// int BswDrv_SysFlashErase(uint32_t address,uint16_t sectorNum);
int BswDrv_SysFlashErase(uint32_t address);

void BswDrv_SysFlashInit(void);


#endif
