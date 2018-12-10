#include "flash_usr.h"
#include "includes.h"
#include "system.h"
#include "flash.h"



SYS_UPDATE_INFO_T   UpdateInfo = {0};

//系统信息写flash
#define FLASH_WRITE_REPEAT_TIMES            ((uint32_t) 5)
int FlashWriteSysInfo(void *pSysInfo, uint16_t size)
{
    #if 1
    uint32_t i = 0;
    SYSTEM_INFO_T systemInfo = {0};
    
    for(i = 0; i < FLASH_WRITE_REPEAT_TIMES; i++)
    {
        FlashWrite(SystemInfoAddr, pSysInfo, size);
        FlashReadByte(SystemInfoAddr, (void *)&systemInfo, sizeof(systemInfo));
        if(memcmp((void *)&systemInfo, pSysInfo, size) == 0)
        {
            break;
        }
    }
    if(FLASH_WRITE_REPEAT_TIMES <= i)
    {
    	printf("写flash错误! \r\n");
        return CL_FAIL;
    }
    #endif
    return CL_OK;
}

int BSPFlashWrite(uint32_t address, uint8_t *pBuffer, uint16_t writeNum)
{
	uint32_t i = 0;
	uint32_t GetData = 0;
	uint8_t timesFlag = 0;
	uint32_t Addr = address;
	uint32_t AddrRead = address;
	
	for(i = 0; i < writeNum; )
    {
		while(1)
		{
			FlashWriteWord(Addr, pBuffer, ((0 == (writeNum % 4)) ? (writeNum / 4) : ((writeNum / 4) + 1)));
			FlashReadByte(AddrRead, (uint8_t*)&GetData, 4);
	//		CL_LOG("qqqqqqq1[%#x], [%#x], [%#x]! \n", address, GetData, *(uint32_t*)pBuffer);
			if((GetData == (*(uint32_t*)pBuffer)))
	        {
				break;
			}
			if(50 < timesFlag++)
			{
				CL_LOG("flash写数据出错[%#x]! \n", address);
				return CL_FAIL;
			}
			Feed_WDT();
		}
		i += 4;
		pBuffer += 4;
		address += 4;
	}
	
    return CL_OK;
}

int FlashTest(void)
{
	uint32_t i = 0;
	uint32_t data = 0x33445566;
	uint32_t address = UPGRADE_INFO;

	{
		fmc_unlock();
		fmc_page_erase(UPGRADE_INFO);//擦除这个扇区
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
		fmc_lock();
	}
    for(i = 0; i < 1024; )
	{
		if(BSPFlashWrite(address, (uint8_t*)&data, 4) != CL_OK)
		{
			CL_LOG("FlashTest failed!!! \n");
			return 1;
		}
		i += 4;
		address += 4;
	}

	CL_LOG("FlashTest OK!!! \n");
	
    return 0;
}



//读取系统信息
void FlashReadSysInfo(void *pSysInfo, uint16_t size)
{
    FlashReadByte(SystemInfoAddr, (void *)pSysInfo, size);

	return;
}



//升级消息写入flash
void WriteUpdateInfo(uint8_t ver,uint16_t segmentNum,uint32_t fsize, uint32_t checkSum)
{
//    SYS_UPDATE_INFO_T updateInfo = {0};
//    
//    updateInfo.updateFlag = 0xaa55;
//    updateInfo.fsize = fsize;
//    updateInfo.checkSum = checkSum;
//    updateInfo.fwVer = ver;
//    updateInfo.segmentNum = segmentNum;
    
//    FlashWrite(SysUpInfoAddr,(void *)&updateInfo,sizeof(updateInfo));
}
