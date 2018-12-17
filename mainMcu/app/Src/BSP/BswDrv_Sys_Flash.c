#include "BswDrv_Sys_Flash.h"
#include "includes.h"


static int FlashWriteWord(uint32_t address,uint8_t *pBuffer,uint16_t writeNum);

static SemaphoreHandle_t FlashMutex = NULL;


void BswDrv_SysFlashInit(void)
{
    FlashMutex = xSemaphoreCreateMutex();
    if(FlashMutex == NULL)
	{
		CL_LOG("flash 创建信号量失败\n");
	}
}



/**********************************************************************************************************
函 数 名: FlashWriteWord
功能说明: 写数据到CPU 内部Flash，使用该函数需要先擦除相应的flash
形    参: address : Flash地址
           pBuffer : 数据缓冲区
            writeNum : 数据大小（单位是4字节）
返 回 值: 0-成功，-1出错
**********************************************************************************************************/
int FlashWriteWord(uint32_t address,uint8_t *pBuffer,uint16_t writeNum)
{
    uint16_t i;
    uint32_t data = 0;
    uint32_t readAddr = address;
	
	for(i = 0;i < writeNum;i++)
    {
        memcpy(&data,pBuffer + i*4,4);
        fmc_word_program(readAddr, data);
        readAddr += 4;
        fmc_flag_clear(FMC_FLAG_BANK0_END);
        fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
        fmc_flag_clear(FMC_FLAG_BANK0_PGERR); 
    }
	
     return CL_OK;
}

/**********************************************************************************************************
函 数 名: FlashRead
功能说明: 从flash读取数据
形    参: address : Flash地址
           pBuffer : 数据缓冲区
            readNum : 数据大小
返 回 值: 0-成功，-1出错
**********************************************************************************************************/
void BswDrv_SysFlashRead(uint32_t address, uint8_t *pBuffer, uint16_t readNum)   	
{
	uint16_t i;
    uint32_t readAddr = address;
	
	for(i = 0; i < readNum; i++)
	{
		pBuffer[i] = REG8(readAddr);
        readAddr += 1;
	}
}


int BswDrv_SysFlashErase(uint32_t address)
{	
	uint32_t offaddr;   //去掉0X08000000后的地址
	uint32_t secpos;	//扇区偏移号

	if(address < FLASH_BASE || ((address+FLASH_PAGE_SIZE) >= (FLASH_BASE+FLASH_SIZE)))  //非法地址
	{
		return CL_FAIL;
	}

	if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//系统已经运行
	{
		xSemaphoreTake(FlashMutex,1000);
	}
    fmc_unlock();

	offaddr = address - FLASH_BASE;		        //实际偏移地址.
	secpos = offaddr/FLASH_PAGE_SIZE;			//扇区地址  
	//
	// CL_LOG("secpos=%d sectorNum=%d \r\n",secpos,sectorNum);

	fmc_page_erase(FLASH_BASE + secpos*FLASH_PAGE_SIZE);//擦除这个扇区
	// for(uint16_t i = 0; i<sectorNum; i++)
	// {
	// 	secpos += i;
	// 	fmc_page_erase(FLASH_BASE + secpos*FLASH_PAGE_SIZE);//擦除这个扇区
	// }

	fmc_flag_clear(FMC_FLAG_BANK0_END);                 //清除所有标志位
	fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
	fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

	fmc_lock();
    if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//系统已经运行
	{
        xSemaphoreGive(FlashMutex);	
	}

	return CL_OK;
}

/**
 * 
 */ 
// int BswDrv_SysFlashErase(uint32_t address,uint16_t sectorNum)
// {	
// 	uint32_t offaddr;   //去掉0X08000000后的地址
// 	uint32_t secpos;	//扇区偏移号

// 	if(address < FLASH_BASE || ((address+sectorNum*FLASH_PAGE_SIZE) >= (FLASH_BASE+FLASH_SIZE)))  //非法地址
// 	{
// 		return CL_FAIL;
// 	}

// 	if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//系统已经运行
// 	{
// 		xSemaphoreTake(FlashMutex,1000);
// 	}
//     fmc_unlock();

// 	offaddr = address - FLASH_BASE;		        //实际偏移地址.
// 	secpos = offaddr/FLASH_PAGE_SIZE;			//扇区地址  
// 	//
// 	CL_LOG("secpos=%d sectorNum=%d \r\n",secpos,sectorNum);

// 	for(uint16_t i = 0; i<sectorNum; i++)
// 	{
// 		secpos += i;
// 		fmc_page_erase(FLASH_BASE + secpos*FLASH_PAGE_SIZE);//擦除这个扇区
// 	}

// 	fmc_flag_clear(FMC_FLAG_BANK0_END);                 //清除所有标志位
// 	fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
// 	fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

// 	fmc_lock();
//     if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//系统已经运行
// 	{
//         xSemaphoreGive(FlashMutex);	
// 	}

// 	return CL_OK;
// }



int BswDrv_SysFlashWrite(uint32_t address,uint8_t *writeBuffer,uint16_t writeLen)
{
	uint16_t writeNum;

	if(address < FLASH_BASE || (address >= (FLASH_BASE+FLASH_SIZE)))  //非法地址
	{
		return CL_FAIL;
	}

	if(writeLen == 0)
	{
		return CL_FAIL;
	}

	if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//系统已经运行
	{
		xSemaphoreTake(FlashMutex,1000);
	}
    fmc_unlock();

	if(writeLen%4 == 0)
	{
		writeNum = writeLen/4;
	}
	else
	{
		writeNum = writeLen/4 + 1;
	}

	FlashWriteWord(address,writeBuffer,writeNum);

	fmc_lock();
    if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//系统已经运行
	{
        xSemaphoreGive(FlashMutex);	
	}

	return CL_OK;
}


#if 0

/**********************************************************************************************************
函 数 名:   FlashWrite
功能说明:   写数据到CPU 内部Flash。具有保存已有的数据内容
形    参:   address : Flash地址，地址最好是4的倍数
            writeBuffer : 数据缓冲区
            writeLen : 数据大小（单位是字节）
返 回 值:   0-成功，-1出错
**********************************************************************************************************/
uint8_t FlashBuffer[FLASH_PAGE_SIZE] = {0};

void FlashWrite(uint32_t address,uint8_t *writeBuffer,uint16_t writeLen)
{
    uint32_t secpos;	   //扇区地址
	uint16_t secoff;	   //扇区内偏移地址(16位字计算)
	uint16_t secremain; //扇区内剩余地址(16位字计算)	 
 	uint16_t i;    
	uint32_t offaddr;   //去掉0X08000000后的地址
    
    if(address < FLASH_BASE || (address >= (FLASH_BASE+FLASH_SIZE)))  //非法地址
	{
		return;
	}
    
    if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//系统已经运行
	{
		xSemaphoreTake(FlashMutex,1000);
	}
    fmc_unlock();
    
    offaddr = address - FLASH_BASE;		        //实际偏移地址.
	secpos = offaddr/FLASH_PAGE_SIZE;			//扇区地址  
	secoff = (offaddr%FLASH_PAGE_SIZE);			//在扇区内的偏移(4个字节为基本单位.)
	secremain = FLASH_PAGE_SIZE - secoff;		//扇区剩余空间大小   
	if(writeLen <= secremain)                   //不大于该扇区范围
	{
		secremain = writeLen;
	}

    while(1) 
	{	
		FlashRead(FLASH_BASE + secpos*FLASH_PAGE_SIZE,FlashBuffer,FLASH_PAGE_SIZE);//读出整个扇区的内容
		for(i = 0;i < secremain;i++)            //校验数据
		{
			if(FlashBuffer[secoff+i] != 0XFF)   //需要擦除  	  
			{
				break;
			}
		}
		if(i < secremain)                       //需要擦除
		{
			fmc_page_erase(FLASH_BASE + secpos*FLASH_PAGE_SIZE);//擦除这个扇区
            fmc_flag_clear(FMC_FLAG_BANK0_END);                 //清除所有标志位
            fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
            fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
            
			for(i=0;i < secremain;i++)          //复制
			{
				FlashBuffer[i+secoff]=writeBuffer[i];	  
			}

			FlashWriteWord(FLASH_BASE + secpos*FLASH_PAGE_SIZE,FlashBuffer,FLASH_PAGE_SIZE/4);//写入整个扇区  
		}
        else 
		{
			FlashWriteWord(address,writeBuffer,secremain/4);//写已经擦除了的,直接写入扇区剩余区间. 	
		}
        
		if(writeLen == secremain)               //写入结束了
		{
			break;
		}
		else                                    //写入未结束
		{
			secpos++;				            //扇区地址增1
			secoff = 0;				            //偏移位置为0 	 
		   	writeBuffer += secremain;  	        //指针偏移 
			address += secremain;	            //写地址偏移	   
		   	writeLen -= secremain;	            //字节(16位)数递减
			if(writeLen > FLASH_PAGE_SIZE)
			{
				secremain = FLASH_PAGE_SIZE;	//下一个扇区还是写不完
			}
			else                                //下一个扇区可以写完了
			{
				secremain = writeLen;
			}
		}	 
	}

    fmc_lock();
    if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//系统已经运行
	{
        xSemaphoreGive(FlashMutex);	
	}
}


#endif
