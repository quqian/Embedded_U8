#include "App_Upgrade.h"




void App_Upgrade_Init(UPGRADE_TYPE type,uint32_t package_num)
{
    OTA_Start(type);
    if (type == UPGRADE_UART || type ==UPGRADE_BLUE )
    {
        upgradeInfo.current_package = 1;
        upgradeInfo.package_num = package_num;
    }
}


/**
 *固件下发
 * @return -1:失败 0-升级完成校验成功 1-固件下成功 2-升级完成校准失败  3-下载文件不对 4-版本一致
 **/  
int App_Upgrade_Write(uint8_t *data,uint16_t datalen)
{
    uint8_t index = data[0];
	uint8_t *pData = data+1;
	uint16_t len = datalen - 1;
    // CL_LOG("index=%d len=%d \n",index,len);
    if(upgradeInfo.current_package == 1 && len >= 32)
    {
        uint8_t upgradeType;
        upgradeInfo.lastIndex = index;
        FW_HEAD_STR *pFwHead = (void*)pData;
        if(pFwHead->aa == 0xAA && pFwHead->five == 0x55){
            
            FW_INFO_STR *pFwInfo = (void*)(pData+16);
            if (0 == memcmp("U8M", pFwInfo->name, 3)) {//主板
				upgradeType = FW_U8;
            }
            else if (0 == memcmp("U8C", pFwInfo->name, 3)) {//刷卡板
                upgradeType = FW_U8_BAK;
            }
            else{
                CL_LOG("fw name=%s, error,exit upgrade.\r\n",pFwInfo->name);
                return 3;
            }
            if(OAT_Init(upgradeType,0,pFwInfo->size,pFwInfo->checkSum,pFwHead->fwVer1) == CL_FAIL)
            {
                CL_LOG("upgrade fail,exit.\n");
				return 4; //
            }

            OAT_FwWrite(upgradeInfo.upgradeFlag,pData+32,len-32);
            upgradeInfo.current_package++;
        }else
        {
            return CL_FAIL;
        }
    }

    if(upgradeInfo.lastIndex == index){
		CL_LOG("repeat transmate pacakge=%d.\n",index);
		return 1;
	}
    upgradeInfo.lastIndex = index;
    OAT_FwWrite(upgradeInfo.upgradeFlag,pData,len);
    if(upgradeInfo.current_package == upgradeInfo.package_num)
    {
        if(OTA_Check(upgradeInfo.upgradeFlag) == CL_FAIL)
        {
            return 2; //下载完成，校验失败
        }
        else
        {
            return CL_OK;//下载完成、校验OK
        }
    }
    else
    {
        upgradeInfo.current_package++;
    }
    return 1;
}

void App_Upgrade_Finish(uint8_t result)
{
    OTA_Finish(result,upgradeInfo.upgradeFlag);
}

