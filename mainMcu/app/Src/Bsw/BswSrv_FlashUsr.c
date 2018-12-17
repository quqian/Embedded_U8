#include "includes.h"
#include "BswSrv_System.h"
#include "BswSrv_FlashUsr.h"



//flash分区
FLASH_PART flash_partition[PART_NUM]=
{
	{AppUpBkpAddr,      66},			//固件升级缓存区
	{SystemInfoAddr,    1}, 			//系统配置
	{SysUpInfoAddr,     1},			    //固件升级头标志
	{OtherInfoAddr,     1},		        //其他-预留信息
};

//系统信息写flash
#define FLASH_WRITE_REPEAT_TIMES            ((uint32_t) 5)

int FlashWriteSysInfo(void *pSysInfo, uint16_t size)
{   
    uint8_t i = 0;
    uint32_t address = flash_partition[SYSCONF_PART].s_base;
    SYSTEM_INFO_T systemInfo = {0};
    
    //擦除扇区
    FlashErase(SYSCONF_PART);

    for(i = 0; i < FLASH_WRITE_REPEAT_TIMES; i++)
    {
        BswDrv_SysFlashWrite(address, pSysInfo, size);
        BswDrv_SysFlashRead(address,(void *)&systemInfo,sizeof(systemInfo));
        if(memcmp((void *)&systemInfo,pSysInfo,size) == 0)
        {
            break;
        }
    }
    if(FLASH_WRITE_REPEAT_TIMES <= i)
    {
    	printf("写flash错误! \r\n");
        return CL_FAIL;
    }
    
    return CL_OK;
}

//读取系统信息
void FlashReadSysInfo(void *pSysInfo, uint16_t size)
{
    BswDrv_SysFlashRead(flash_partition[SYSCONF_PART].s_base, (void *)pSysInfo, size);
	return;
}



void FlashErase(PARTITION n)
{
    uint32_t address = flash_partition[n].s_base;
    uint32_t count = flash_partition[n].s_count;
    
    for(uint8_t i = 0;i<count;i++)
    {
        BswDrv_SysFlashErase(address+i*FLASH_PAGE_SIZE);
    }
}




