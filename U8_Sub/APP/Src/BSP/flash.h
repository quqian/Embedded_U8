#ifndef __FLASH_H__
#define __FLASH_H__

#include "includes.h"


#define IS_PROTECTED_AREA(addr)            	((uint8_t)(((addr < 0x08000000) && (addr > 0x08010000))? 1 : 0))
#define FLASH_DELAY_MS           			DelayMsWithNoneOs


void FlashInit(void);
int FlashReadByte(uint32_t address,uint8_t *pBuffer,uint16_t readNum);
int FlashWriteWord(uint32_t address,uint8_t *pBuffer,uint16_t writeNum);
int FlashWrite(uint32_t address,uint8_t *writeBuffer,uint16_t writeLen);

#endif
