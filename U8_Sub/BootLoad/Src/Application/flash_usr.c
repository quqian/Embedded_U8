#include "flash_usr.h"
#include "includes.h"
#include "system.h"
#include "flash.h"




SYS_UPDATE_INFO_STR updateInfo;


int Upgrade_FlashCheck(void)
{
	uint16_t checksum = 0;
	uint8_t  rbyte = 0;
	uint32_t i = 0;

	FeedWatchDog();
	for(i = 0; i < updateInfo.size; i++)
	{
		//FeedWatchDog();
		FlashReadByte(AppUpBkpAddr + i, (void *)&rbyte, 1);
		checksum += rbyte;
        //printf("boot checksum[%#x], [%#x]\r\n", checksum, rbyte);
	}
	
	if(checksum != updateInfo.checkSum)  
	{
        printf("校验出错!!![%#x], [%#x]\r\n", checksum, updateInfo.checkSum);
		return 1;
	}
	
	return 0;
}

uint8_t buffer[FLASH_PAGE_SIZE];
int UpdateFromAppBkp(uint32_t fsize, uint16_t checksum)
{
//	uint8_t* p = NULL;
	uint32_t i = 0;
	uint16_t ck = 0;
	uint16_t page;
	uint16_t lenth;
	uint8_t byte;
	SYS_UPDATE_INFO_STR UpdateInfoStr;

	FeedWatchDog();
	lenth = fsize / FLASH_PAGE_SIZE + 1;
		
	//计算备份App程序checksum
	if(Upgrade_FlashCheck() != 0)
	{
		return -1;
	}

	fmc_unlock();
	for (i = 0; i < (uint32_t)(APP_FW_SIZE / FLASH_PAGE_SIZE); i++)     //擦除APP程序区 
    {
		fmc_page_erase(AppFlashAddr + i * FLASH_PAGE_SIZE);//擦除这个扇区
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }
    fmc_lock();
	
	//擦除App存储页面并写入数据
	for(page = 0; page < lenth; page++)
	{
		FeedWatchDog();
		FlashReadByte((uint32_t)(AppUpBkpAddr + page * FLASH_PAGE_SIZE), buffer, 1024);
		FlashWrite((uint32_t)(AppFlashAddr + page * FLASH_PAGE_SIZE), buffer, 1024);
	}
	
	ck = 0;
	//写入数据完毕,计算App更新程序后的校验
	for(i = 0; i < fsize; i++)
	{
		FlashReadByte((uint32_t)(AppFlashAddr + i), &byte, 1);
		ck += byte;
    }
	
	//如果校验不等于升级程序的校验,返回失败
    if(ck == checksum)
	{
		printf("checksum ok, update success!\r\n");
		for(i = 0; i < 8; i++)
		{
			UpdateInfoStr.updateFlag = 0;
			FlashWrite((uint32_t)UPGRADE_INFO, (uint8_t*)&UpdateInfoStr, sizeof(UpdateInfoStr));
			FlashReadByte((uint32_t)UPGRADE_INFO, (void *)&updateInfo, sizeof(updateInfo));
            DelayMsWithNoneOs(20);
			FlashReadByte((uint32_t)UPGRADE_INFO, (void *)&UpdateInfoStr, sizeof(UpdateInfoStr));
			if((0 == updateInfo.updateFlag) && (0 == UpdateInfoStr.updateFlag))
			{
				break;
			}
		}
		
		return 0;
	}
	else
	{
		printf("checksum fail, update failed!\r\n");
		
		return -1;
	}
}




