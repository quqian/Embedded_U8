/*
 * @Author: zhoumin 
 * @Date: 2018-10-23 09:11:01 
 * @Last Modified by: zhoumin
 * @Last Modified time: 2018-11-08 17:27:25
 */
#include "includes.h"
#include "BswSrv_System.h"
#include "BswSrv_FwUpgrade.h"
#include "BswSrv_FlashUsr.h"
#include "App_CardBoardProto.h"
#include "BswDrv_sc8042b.h"
#include "BswSrv_NetTask.h"

TaskHandle_t CardUpgradeHandle_t  = NULL;

UPGRADE_INFO_STR_T upgradeInfo;

extern FLASH_PART flash_partition[PART_NUM];


void OTA_Start(UPGRADE_TYPE type)
{
	Sc8042bSpeech(VOIC_START_UPGRADE);

    if(type == UPGRADE_FTP || type == UPGRADE_HTTP)
    {
        GlobalInfo.upgradeFlag = OTA_NET;
    }else if(type == UPGRADE_UART)
    {
        GlobalInfo.upgradeFlag = OTA_UART;
    }else if(type == UPGRADE_BLUE)
    {
        GlobalInfo.upgradeFlag = OTA_BLUE;
    }
    GlobalInfo.upgradeTime = osGetTimes();

    memset(&upgradeInfo,0,sizeof(upgradeInfo));

    if(type == UPGRADE_FTP || type == UPGRADE_HTTP)
    {
        //如果是ftp或者http下载先关闭网络
        BswSrv_CloseNetWork();
    }
}

int OAT_Init(uint8_t upgradeFlag,uint32_t startAddrs,uint32_t fileSize,uint16_t checkSum,uint8_t fwVer)
{    
	if(FW_U8 == upgradeFlag && fwVer == VERSON_SER){
		CL_LOG("fw_version is equal,no need to upgrade.\n");
		return CL_FAIL;
	}
	
    upgradeInfo.fsize = fileSize;
    upgradeInfo.upgradeFlag = upgradeFlag;
    upgradeInfo.checkSum = checkSum;
    upgradeInfo.fw_version = fwVer;
    upgradeInfo.startAddrs = startAddrs;
    upgradeInfo.wirteSize = 0;

    //固件缓存区擦除
    uint8_t sectorNum = fileSize/FLASH_PAGE_SIZE +1;
    
     for(uint8_t i = 0;i<sectorNum;i++)
    {
        BswDrv_SysFlashErase(flash_partition[FIRMWARE_PART].s_base+i*FLASH_PAGE_SIZE);
    }

    CL_LOG("OAT_Init ,fileSize=%d  checkSum=%X \r\n",fileSize,checkSum);
	
	return CL_OK;
}

//
void OAT_FwWrite(uint8_t type,uint8_t *data,uint16_t len)
{
    uint32_t baseAddr = flash_partition[FIRMWARE_PART].s_base + upgradeInfo.startAddrs;
	
	BswDrv_SysFlashWrite(baseAddr+upgradeInfo.wirteSize, data, len);
	upgradeInfo.wirteSize += len;
    
}

int Upgrade_FlashCheck(uint8_t type)
{
    uint16_t checksum = 0;
    uint32_t readAddr = flash_partition[FIRMWARE_PART].s_base + upgradeInfo.startAddrs;

    for(uint32_t i = 0; i < upgradeInfo.fsize; i++)
	{
		checksum += REG8(readAddr);
        readAddr += 1;
	}
    CL_LOG("checksum=%X.\n",checksum);
    if(checksum != upgradeInfo.checkSum)  
        return CL_FAIL;
	
	return CL_OK;
}


int OTA_Check(uint8_t type)
{
    if(Upgrade_FlashCheck(type) != CL_OK)
    {
        CL_LOG("FlashCheck failed \r\n");
        return CL_FAIL;
    }
    CL_LOG("FlashCheck success.\r\n");
    SYS_UPDATE_HEAD_INFO_T upgradeHeadInfo;
    BswDrv_SysFlashRead(SysUpInfoAddr,(void*)&upgradeHeadInfo,sizeof(upgradeHeadInfo));

    upgradeHeadInfo.headFlag = 0x55AA;
    upgradeHeadInfo.fw[type].upgradeFlag = 1;
    upgradeHeadInfo.fw[type].fsize = upgradeInfo.fsize;
    upgradeHeadInfo.fw[type].startAddrs = upgradeInfo.startAddrs;
    upgradeHeadInfo.fw[type].checkSum = upgradeInfo.checkSum;
    upgradeHeadInfo.fw[type].fwVer = upgradeInfo.fw_version;

    FlashErase(UPGRADEHEAD_PART);
    BswDrv_SysFlashWrite(SysUpInfoAddr,(void *)&upgradeHeadInfo,sizeof(upgradeHeadInfo));
	
    if(type == FW_U8)
    {
        CL_LOG("U8_Main upgrade success,reboot will ok.\r\n");
    }
    else if(type == FW_U8_BAK)
    {
        CL_LOG("U8_Sub OTA success, will start upgrade task.\r\n");
        BswSrv_StartCardBoard_UpgradeTask();
    }

    return CL_OK;
}


void OTA_Finish(uint8_t result,uint8_t type)
{
    GlobalInfo.upgradeFlag = OTA_NONE;

    //U8主板升级成功-
    if(result == 0)
    {
         Sc8042bSpeech(VOIC_SUCCESS);
        if(type == FW_U8)
        {
            osDelay(1000);
            BswSrv_SystemReboot();
        }
    }
}


//重发次数
#define RETRY_TIMERS    5
void CardBoard_UpgradeTask(void)
{
    uint8_t buf[CARD_UPGRADE_SIZE];
    FW_HEAD_INFO_T info;
    uint8_t index = 0;
    uint32_t package = 0;
    uint32_t readAddr = 0;
    uint32_t remain = 0;
    uint16_t read_len ;
    uint32_t current_bin_file_index = 0;
    uint8_t i = 0;
    BaseType_t result;
    uint32_t value;

    CL_LOG("刷卡版升级任务启动..\r\n");

    osDelay(1000);

    if(BswSrv_Upgrade_ReadHeadInfo(FW_U8_BAK,&info) != CL_OK)
    {
        CL_LOG("头信息认证失败.\r\n");
        goto EXIT;
    }
    package = info.fsize/CARD_UPGRADE_SIZE;
    if(info.fsize % CARD_UPGRADE_SIZE != 0)
    {
        package ++;
    }
    for(i = 0;i<RETRY_TIMERS;i++)
    {
        //发送升级命令
        App_CB_SendStartUpgrade(info.fsize,package,info.checkSum,info.fwVer);
        CL_LOG("SendStartUpgrade...\r\n");
        //等待响应
        result = xTaskNotifyWait(0,                
                               (uint32_t  )0xFFFFFFFF,            //退出函数的时候清除所有的bit
                               (uint32_t*  )&value,               //保存任务通知值
                               (TickType_t  )2000); 
        if(result == pdTRUE) {
            CL_LOG("recv req ack .\r\n");
            break;
        }
    }
    if(i >= RETRY_TIMERS)
    {
        CL_LOG("no recv req ack.. \r\n");
        goto EXIT;
    }

    readAddr = flash_partition[FIRMWARE_PART].s_base + info.startAddrs;
    remain = info.fsize;

    //发送固件
	while(remain)
	{
		if(remain >= CARD_UPGRADE_SIZE){
            read_len = CARD_UPGRADE_SIZE;
        }else{
            read_len = remain;
        }
        remain = remain-read_len;
        for(int i = 0; i < read_len;i++){
            buf[i] = REG8(readAddr);
            readAddr += 1;
        }

        for(i = 0;i<RETRY_TIMERS;i++)
        {
            //发送数据
            App_CB_DownFW(index,buf,read_len);
            //等待响应
            result = xTaskNotifyWait(0,
                               (uint32_t  )0xFFFFFFFF,  
                               (uint32_t*  )&value,  
                               (TickType_t  )2000); 
            if(result == pdTRUE && index == value) {
                break;
            }
            if(index != value){
                CL_LOG("index error..index=%d value=%d \r\n",index,value);
            }
        }
        if(i >= RETRY_TIMERS)
        {
            CL_LOG("no recv fw ack..index=%d \r\n",index);
            goto EXIT;
        }
    
        index++;
        current_bin_file_index+=read_len;
		osDelay(50);
	}
	CL_LOG("U8 CardBoard upgrade success.\r\n");
    //修改头信息
    SYS_UPDATE_HEAD_INFO_T upgradeHeadInfo;
    BswDrv_SysFlashRead(SysUpInfoAddr,(void*)&upgradeHeadInfo,sizeof(upgradeHeadInfo));
    memset(&upgradeHeadInfo.fw[FW_U8_BAK],0,sizeof(upgradeHeadInfo.fw[FW_U8_BAK]));
    BswDrv_SysFlashErase(SysUpInfoAddr);
    BswDrv_SysFlashWrite(SysUpInfoAddr,(void*)&upgradeHeadInfo,sizeof(upgradeHeadInfo));

EXIT:
    CL_LOG("刷卡版升级退出..\r\n");
    CardUpgradeHandle_t = NULL;
	vTaskDelete(NULL);
}


void BswSrv_Upgrade_SendNotify(uint8_t transAction)
{
   if(CardUpgradeHandle_t != NULL)
   {
       xTaskNotify((TaskHandle_t  )CardUpgradeHandle_t,    //接收任务通知的任务句柄
                   (uint32_t    )transAction,            //任务通知值
                   (eNotifyAction  )eSetValueWithOverwrite);  //覆写的方式发送任务通知
   }
}

int BswSrv_Upgrade_ReadHeadInfo(uint8_t type,FW_HEAD_INFO_T *info)
{
    SYS_UPDATE_HEAD_INFO_T upgradeHeadInfo;
    BswDrv_SysFlashRead(SysUpInfoAddr,(void *)&upgradeHeadInfo,sizeof(upgradeHeadInfo));

    if(upgradeHeadInfo.headFlag == 0x55AA && upgradeHeadInfo.fw[type].upgradeFlag == 1)
    {
        if(info != NULL)
        {
            memcpy(info,&upgradeHeadInfo.fw[type],sizeof(FW_HEAD_INFO_T));
        }
        return CL_OK;
    }
    return CL_FAIL;
}

void BswSrv_StartCardBoard_UpgradeTask(void)
{
    if(CardUpgradeHandle_t == NULL && GlobalInfo.CBInitOK)
    {
        xTaskCreate((TaskFunction_t)CardBoard_UpgradeTask, "UpgradeTask", 256, NULL, 7, &CardUpgradeHandle_t);    
    }
    else
    {
        CL_LOG("升级任务正在运行,或者刷卡版不在线.\r\n");
    }
}




