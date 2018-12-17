#include "includes.h"
#include "upgrade.h"



uint8_t FlashBuffer[FLASH_PAGE_SIZE] = {0};
	
SYS_UPDATE_INFO_T updateInfo;



int UpdateFromAppBkp(uint32_t startAddr,uint32_t fsize, uint16_t checksum)
{
	uint16_t ck = 0;
	uint16_t page = fsize/FLASH_PAGE_SIZE + 1;
	
	startAddr += AppUpBkpAddr;	//备份区域U8主板升级地址
	
	printf("startAddr=%X fsize=%d checksum=%X \r\n",startAddr,fsize,checksum);
	
	//检测备份区域的数据是否正确
	for(uint32_t i = 0;i<fsize;i++)
	{
		ck += REG8(startAddr+i);
	}
	
	if(ck != checksum)
	{
		printf("备份区域数据检测失败..\r\n");
		return CL_FAIL;
	}
	
	printf("正在拷贝数据到APP区域.\r\n");

	for(uint16_t i = 0; i < page; i++)
	{	
		//擦除app区域
		BswDrv_SysFlashErase(AppFlashAddr + i * FLASH_PAGE_SIZE);
		
		//从备份区读取数据
		BswDrv_SysFlashRead(startAddr + i * FLASH_PAGE_SIZE,FlashBuffer,FLASH_PAGE_SIZE);
		
		//写数据到app区域
		BswDrv_SysFlashWrite(AppFlashAddr + i * FLASH_PAGE_SIZE,FlashBuffer,FLASH_PAGE_SIZE);
		
	}
	
	ck = 0;
	//检测APP区域的数据
	for(uint32_t i = 0;i<fsize;i++)
	{
		ck += REG8(AppFlashAddr+i);
	}
	
	if(ck != checksum)
	{
		printf("App checksum fail, update failed!\r\n");
		return CL_FAIL;
	}
	
	printf("checksum ok, update success!\r\n");
	
	//修改头信息
	memset(&updateInfo.fw[FW_U8],0,sizeof(updateInfo.fw[FW_U8]));
	
	BswDrv_SysFlashErase(SysUpInfoAddr);
	BswDrv_SysFlashWrite(SysUpInfoAddr,(void*)&updateInfo,sizeof(updateInfo));
	
	return CL_OK;
}
