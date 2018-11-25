#include "flash.h"
#include "includes.h"
#include "system.h"




/*****************************************************************************
** Function name:   	FlashReadByte
** Descriptions:        从flash读取数据
** input parameters:    address : Flash地址
           				pBuffer : 数据缓冲区
            			readNum : 数据大小
** Returned value:	    0-成功，-1出错
** Author:              quqian
*****************************************************************************/
int FlashReadByte(uint32_t address, uint8_t *pBuffer, uint16_t readNum)   	
{
	uint32_t i;
    uint32_t readAddr = address;
	
//    CL_LOG("address = [0x%x], readNum = [0x%x]\n", address, readNum);
	for(i = 0; i < readNum; i++)
	{
		pBuffer[i] = REG8(readAddr);
        readAddr += 1;
//		CL_LOG("readAddr = [0x%x], pBuffer[i] = [0x%x]\n", readAddr, pBuffer[i]);
	}

	return 0;
}


/*****************************************************************************
** Function name:   	FlashWriteWord
** Descriptions:        写数据到芯片内部Flash, 使用该函数需要先擦除相应的flash
** input parameters:    address : Flash地址
           				pBuffer : 数据缓冲区
            			writeNum : 数据大小(4的倍数)
** Returned value:	    0-成功，-1出错
** Author:              quqian
*****************************************************************************/
int FlashWriteWord(uint32_t address, uint8_t *pBuffer, uint16_t writeNum)
{
    uint32_t i;
	uint32_t OutTimeFlag = 0;
    uint32_t readAddr = address;
	
	fmc_unlock();
	for(i = 0; i < writeNum; i++)
    {
    	OutTimeFlag = 0;
    	fmc_flag_clear(FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_END);
        if (FMC_READY == fmc_word_program(readAddr, *((uint32_t *)(pBuffer + i * 4)))) 
		{
            readAddr += 4;
        }
		else
		{
            while(1)
			{
				FLASH_DELAY_MS(1);
				if(1000 < OutTimeFlag++)
				{
					CL_LOG("写flash超时, [0x%x], 错误!!! \n", readAddr);
					break;
				}
			}
        }
	}
	fmc_lock();
    
	return CL_OK;
}


/**********************************************************************************************************
函 数 名:   FlashWrite
功能说明:   写数据到CPU 内部Flash。具有保存已有的数据内容
形    参:   address : Flash地址，地址最好是4的倍数
            writeBuffer : 数据缓冲区
            writeLen : 数据大小（单位是字节）
返 回 值:   0-成功，-1出错
**********************************************************************************************************/
uint8_t FlashBuffer[FLASH_PAGE_SIZE] = {0};
int FlashWrite(uint32_t address, uint8_t *writeBuffer, uint16_t Len)
#if 1
{
    uint32_t secpos;	   		//扇区地址
	uint16_t secoff;	   		//扇区内偏移地址(16位字计算)
	uint16_t secremain; 		//扇区内剩余地址(16位字计算)	 
 	uint32_t i;    
	uint32_t offaddr;   		//去掉0X08000000后的地址
	uint16_t writeLen = Len;
	
	FeedWatchDog();
    if (IS_PROTECTED_AREA(address)) 
	{	//非法地址
		CL_LOG("写入flash起始地址[0x%x], 错误!!! \n", address);
        return CL_FAIL;
    }
//    if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//系统已经运行
//	{
//		xSemaphoreTake(FlashMutex,1000);
//	}
    fmc_unlock();
    
    offaddr = address - FLASH_BASE;		        	//实际偏移地址.
	secpos = offaddr / FLASH_PAGE_SIZE;				//扇区地址  
	secoff = (offaddr % FLASH_PAGE_SIZE);			//在扇区内的偏移(4个字节为基本单位.)
	secremain = FLASH_PAGE_SIZE - secoff;			//扇区剩余空间大小   
	if(writeLen <= secremain)                   	//不大于该扇区范围
	{
		secremain = writeLen;
	}

    while(1) 
	{
        FeedWatchDog();
		FlashReadByte(FLASH_BASE + secpos * FLASH_PAGE_SIZE, FlashBuffer, FLASH_PAGE_SIZE);		//读出整个扇区的内容
		for(i = 0; i < secremain; i++)            	//校验数据
		{
			if(FlashBuffer[secoff + i] != 0XFF)   	//需要擦除  	  
			{
				CL_LOG("需要擦除.\n");
				break;
			}
		}
		if(i < secremain)                       	//需要擦除
		{
			fmc_page_erase(FLASH_BASE + secpos * FLASH_PAGE_SIZE);//擦除这个扇区
			fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
			for(i = 0; i < secremain; i++)          //复制
			{
				FlashBuffer[i + secoff] = writeBuffer[i];
			}

			FlashWriteWord(FLASH_BASE + (secpos * FLASH_PAGE_SIZE), FlashBuffer, (FLASH_PAGE_SIZE / 4));//写入整个扇区  
		}
        else 
		{
			FlashWriteWord(address, writeBuffer, ((0 == (secremain % 4)) ? (secremain / 4) : ((secremain / 4) + 1)));	//写已经擦除了的, 直接写入扇区剩余区间. 	
		}
        
		if(writeLen == secremain)               //写入结束了
		{
			CL_LOG("写完成功.\n");
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
				secremain = FLASH_PAGE_SIZE;		//下一个扇区还是写不完
			}
			else                                //下一个扇区可以写完了
			{
				secremain = writeLen;
			}
		}	 
	}

    fmc_lock();
//    if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//系统已经运行
//	{
//        xSemaphoreGive(FlashMutex);	
//	}

    return CL_OK;
}
#endif


