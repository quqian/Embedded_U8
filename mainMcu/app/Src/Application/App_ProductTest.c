#include "includes.h"
#include "BswSrv_System.h"
#include "App_ProductTest.h"
#include "BswDrv_sc8042b.h"
#include "BswDrv_Debug.h"
#include "BswDrv_Rtc.h"
#include "BswSrv_FlashUsr.h"
#include "App_Upgrade.h"
#include "BswDrv_GPIO.h"
#include "BswSrv_Air720.h"
#include "BswSrv_WG215.h"
#include "BswSrv_NetTask.h"

static uint8_t ct_sn = 0;
static TimerHandle_t timerHandle;
static uint8_t isTestWifiFlag = 0;
static uint8_t isTest4GFlag = 0;

uint8_t isCheckRF433 = 0;//0-Î´Æ¥Åäµ½ 1--Æ¥Åä³É¹¦  2--Æ¥ÅäÊ§°Ü
uint32_t TestRF433Addres;
uint32_t readRF433Addr;

static void App_ProTest_FrameHandle(uint8_t *data,uint16_t len);

void BswSrv_ProTest_RxData_Callback(uint8_t *data,uint16_t len)
{
	App_ProTest_FrameHandle(data,len);
}


int App_ProTest_SendPkt(OUT_PKT_STR *pFrame, uint16_t len)
{
    pFrame->head.aa = 0xaa;
    pFrame->head.five = 0x55;
    pFrame->head.len = 4 + len;
    pFrame->head.ver = 1;
	pFrame->head.sn = ct_sn++;
    pFrame->data[len] = BswSrv_Tool_CheckSum((void*)&pFrame->head.ver, len + 3);

	BswSrv_ProTest_SendData((void*)pFrame, sizeof(OUT_NET_HEAD_STR) + len + 1);

	return CL_OK;
}

int App_ProTest_SendCardInfo(uint8_t *cardId,uint8_t flag)
{
	uint8_t pBuff[64] = {0};
	OUT_PKT_STR *pFrame = (void*)pBuff;
	pFrame->head.cmd = DEBUG_CMD_CARD2;
	
	pFrame->data[0] = (flag==3 ? 0 : 1);
	memcpy(&pFrame->data[1],cardId,16);

	return App_ProTest_SendPkt(pFrame,17);
}

int App_ProTest_SendKeyInfo(uint8_t keyValue)
{
	uint8_t pBuff[64] = {0};
	OUT_PKT_STR *pFrame = (void*)pBuff;
	pFrame->head.cmd = DEBUG_CMD_KEY;
	pFrame->data[0] = 0x01;
	pFrame->data[1] = keyValue;
	
	return App_ProTest_SendPkt(pFrame,2);
}

int App_ProTest_SendRF433Rst(uint8_t ret,uint32_t address)
{
	uint8_t pBuff[64] = {0};
	OUT_PKT_STR *pFrame = (void*)pBuff;
	pFrame->head.cmd = DEBUG_CMD_RF433_TEST;
	pFrame->data[0] = 0x01;
	pFrame->data[1] = ret;
	pFrame->data[2] = (address >> 16)&0xFF;
	pFrame->data[3] = (address >> 8)&0xFF;
	pFrame->data[4] = address&0xFF;
	return App_ProTest_SendPkt(pFrame,5);
}

int App_ProTest_SendNetTestACK(uint8_t cmd,uint8_t ret)
{
	uint8_t pBuff[64] = {0};
	OUT_PKT_STR *pFrame = (void*)pBuff;
	pFrame->head.cmd = cmd;
	pFrame->data[0] = 0;
	pFrame->data[1] = ret;
	return App_ProTest_SendPkt(pFrame,2);
}

void App_ProTest_4GTestFinish(uint8_t result)
{
	if(result == 0)
	{
		xTimerStop(timerHandle,0);
	}
	isTest4GFlag = 0;
	App_ProTest_SendNetTestACK(DEBUG_CMD_SERVER_COM,result);
}

void App_ProTest_WIFITestFinish(uint8_t result)
{
	if(result == 0)
	{
		xTimerStop(timerHandle,0);
	}
	isTestWifiFlag = 0;
	App_ProTest_SendNetTestACK(DEBUG_CMD_WIFI_SER,result);
}

int App_ProTest_Flash(void)
{
	uint32_t write = 0xAA55A55A;
	FlashErase(RESEVERD_PART);
	uint32_t addroff = 0;
	for(uint16_t i = 0;i<256;i++)
	{
		BswDrv_SysFlashWrite(OtherInfoAddr+addroff, (uint8_t*)&write, 4);
		addroff += 4;
	}
	uint32_t read;
	addroff = 0;
	for(uint16_t i = 0;i<256;i++)
	{
		BswDrv_SysFlashRead(OtherInfoAddr+addroff,(uint8_t*)&read,4);
		addroff += 4;
		if(read != write){
			return CL_FAIL;
		}
	}
	CL_LOG("flash test ok.\r\n");
	return CL_OK;
}

static void ProTest_Timeout(TimerHandle_t xTimer)
{
	if(xTimer == timerHandle)
	{
		if(isTest4GFlag == 1)
		{
			isTest4GFlag = 0;
			App_ProTest_4GTestFinish(1);
		}
		if(isTestWifiFlag == 1)
		{
			isTestWifiFlag = 0;
			App_ProTest_WIFITestFinish(1);
		}
	}
	CL_LOG("ProTest_Timeout..\r\n");
}

void App_ProTest_FrameHandle(uint8_t *data,uint16_t len)
{
    uint16_t dataLen = 1;
    OUT_PKT_STR *pBuff = (void*)data;

	// if(pBuff->head.cmd != DEBUG_CMD_FW_DOWNLOAD){
	// 	PrintfData("DebugRecvProc", (void*)pBuff, len);
	// }

    switch (pBuff->head.cmd) {
		case DEBUG_CMD_TEST:	//ÇëÇó²âÊÔ
			if(pBuff->data[0] == 0)
			{
				GlobalInfo.isTest = 1;
				CL_LOG("in Test mode.\r\n");
			}else{
				GlobalInfo.isTest = 0;
				CL_LOG("exit Test mode.\r\n");
			}
			pBuff->data[1] = 0;
			dataLen = 2;
			break;
		case DEBUG_CMD_KEY: //°´¼ü²âÊÔ
			GlobalInfo.isTest = 1;
			pBuff->data[0] = 0;
			break;
		//case DEBUG_CMD_CARD1:	
		case DEBUG_CMD_CARD2://Ë¢¿¨²âÊÔ
			GlobalInfo.isTest = 1;
			return;
		case DEBUG_CMD_TRUMPTE://À®°È²âÊÔ
			CL_LOG("DEBUG_CMD_TRUMPTE \n");
			Sc8042bSpeech(VOIC_WELCOME);
			pBuff->data[0] = 0;
			break;
		case DEBUG_CMD_LED:	//LEDµÆ²âÊÔ
			if(pBuff->data[1] == 0)
			{
				LED_R_OFF();
				LED_G_OFF();
				LED_B_OFF();
			}else{
				LED_R_ON();
				LED_G_ON();
				LED_B_ON();
			}	
			pBuff->data[0] = 0;
			break;
		case DEBUG_CMD_DOOR_STATE://ÃÅ½û×´Ì¬²âÊÔ
			DOOR_HIGH();
			osDelay(100);
			if(READ_DOOR_STATE() == 1){
				pBuff->data[1] = 0;
			}else{
				pBuff->data[1] = 1;
			}
			dataLen = 2;
			break;
		case DEBUG_CMD_SIM: //2/4G²âÊÔ
			CL_LOG("4GÐÅºÅ²âÊÔ. \r\n");
			BswSrv_Air720_GetSignal(2);
			pBuff->data[0] = 0;
			pBuff->data[1] = GlobalInfo.simSignal;
			memcpy(&pBuff[2],&GlobalInfo.iccid[0],ICCID_LEN);
			dataLen = 22;
	
			break;
		case DEBUG_CMD_SERVER_COM://4GÁªÍø²âÊÔ
			CL_LOG("4GÁªÍø²âÊÔÇëÇó isTest4GFlag=%d\r\n",isTest4GFlag);
			if(isTest4GFlag == 0)
			{
				isTest4GFlag = 1;//2·ÖÖÓºóÐèÒª¸Ä³É0
				xTimerStart(timerHandle,0);
				if(BswSrv_ResetNetWork(NETTYPE_GPRS) == CL_OK)
				{
					App_ProTest_4GTestFinish(0);
				}else{
					CL_LOG("ÕýÔÚÇÐ»»4GÉÏÍø.\r\n");
				}
			}
			return ;
		case DEBUG_CMD_WIFI_CONFIG:	//wifi²âÊÔ
			memset(SystemInfo.WifiName,0,sizeof(SystemInfo.WifiName));
			memset(SystemInfo.WifiPasswd,0,sizeof(SystemInfo.WifiPasswd));
			
			sprintf(SystemInfo.WifiName,"\"%s\"",&pBuff->data[0]);
			sprintf(SystemInfo.WifiPasswd,"\"%s\"",&pBuff->data[32]);

			FlashWriteSysInfo(&SystemInfo,sizeof(SystemInfo));
			CL_LOG("set wifi ssid=%s  paswd=%s \r\n",SystemInfo.WifiName,SystemInfo.WifiPasswd);
			pBuff->data[0] = 0;
			break;
		case DEBUG_CMD_WIFI_SER://wifiÁªÍø²âÊÔ
			CL_LOG("wifiÁªÍø²âÊÔ date[0]=%d \r\n",pBuff->data[0]);
			if(pBuff->data[0] == 0x00 && isTestWifiFlag == 0)
			{
				isTestWifiFlag = 1;//2·ÖÖÓºóÐèÒª¸Ä³É0
				xTimerStart(timerHandle,0);
				if(BswSrv_ResetNetWork(NETTYPE_WIFI) == CL_OK)
				{
					App_ProTest_WIFITestFinish(0);
				}else{
					CL_LOG("ÕýÔÚÇÐ»»wifiÉÏÍø.\r\n");
				}
			}
			return;
		case DEBUG_CMD_WIFI_CSQ://wifiÐÅºÅÖµ
			if(pBuff->data[0] == 0x00)
			{
				pBuff->data[0] = 0;
			}
			break;
		case DEBUG_CMD_DOOR_TEST:	//ÃÅËø¿ØÖÆ
			CL_LOG("ÃÅËø¿ØÖÆ.\r\n");
			BswDrv_OpenDoor(TYPE_SYNC);
			pBuff->data[0] = 0;
			break;
		case DEBUG_CMD_RF433_TEST:	//ÑÌ¸Ð
			TestRF433Addres = (uint32_t)( (pBuff->data[0]<<16)|(pBuff->data[1]<<8)|pBuff->data[2]);
			CL_LOG("Test RF433,address=%X \r\n",TestRF433Addres);
			pBuff->data[0] = 0;
			pBuff->data[1] = 0;
			dataLen = 2;
			break;
		case DEBUG_CMD_RS232_TEST:	//232²âÊÔ
			if(GlobalInfo.CBInitOK)
			{
				pBuff->data[0] = 0;
			}else{
				pBuff->data[0] = 1;
			}
			break;
		case DEBUG_CMD_GETSVER:		//»ñÈ¡°æ±¾ºÅ
			CL_LOG("DEBUG_CMD_GETSVER data[0]=%d \n",pBuff->data[0]);
			if(pBuff->data[0] == 0){//Ö÷°æ±¾
				pBuff->data[1] = FW_VERSION;
				pBuff->data[2] = FW_VERSION_SUB1;
				pBuff->data[3] = FW_VERSION_SUB2;
			}else{//¸±°æ±¾
				pBuff->data[1] = 0x00;
				pBuff->data[2] = 0x00;
				pBuff->data[3] = GlobalInfo.CBVerson;
			}
			dataLen = 4;
			break; 
		case DEBUG_CMD_BLUE:		//À¶ÑÀ
			CL_LOG("À¶ÑÀ×®ºÅ.\r\n");
			if(GlobalInfo.WG215InitOK)
			{
				pBuff->data[0] = GlobalInfo.BlueInitOk;
			}else{
				pBuff->data[0] = 2;
			}
			dataLen = 1;
			break;
		case DEBUG_CMD_FLASHTEST:	//Flash¶ÁÐ´
			CL_LOG("ÉèÖÃ×®ºÅ.\r\n");
			if(App_ProTest_Flash() == CL_OK)
			{
				pBuff->data[0] = 0;
			}else{
				pBuff->data[0] = 1;
			}
			dataLen = 1;
			break;
		case DEBUG_CMD_ICode:		//ÉèÖÃ×¢²áÂë
		case DEBUG_CMD_ICode2:
			CL_LOG("ÉèÖÃ×¢²áÂë.\r\n");
			PrintfData("ÉèÖÃ×¢²áÂë=",pBuff->data,6);
			SystemInfo.idCode[0] = 0;
			SystemInfo.idCode[1] = 0;
			memcpy(&SystemInfo.idCode[2],pBuff->data,6);
			FlashWriteSysInfo(&SystemInfo,sizeof(SystemInfo));
			pBuff->data[0] = 0;
			break;
        case DEBUG_CMD_SN:	//ÉèÖÃ×®ºÅ
			CL_LOG("ÉèÖÃ×®ºÅ.\r\n");
			if(memcmp(SystemInfo.stationId,pBuff->data,CHARGER_SN_LEN) != 0){
				memcpy(SystemInfo.stationId,pBuff->data,CHARGER_SN_LEN);
				memset(SystemInfo.idCode,0,sizeof(SystemInfo.idCode));
				FlashWriteSysInfo(&SystemInfo,sizeof(SystemInfo));
			}
			pBuff->data[0] = 0;
            break;
		case DEBUG_CMD_WRITEPCB:	//Ð´PCB±àºÅ
			if(pBuff->data[0] == 0){
				dataLen = 2;
				memcpy(SystemInfo.pcbSn,&pBuff->data[1],8);
				if(FlashWriteSysInfo(&SystemInfo,sizeof(SystemInfo)) == CL_OK){
					pBuff->data[1] = 0;
				}else{
					pBuff->data[1] = 1;
				}
			}
			break;
		case DEBUG_CMD_READPCB:		//¶ÁPCB±àºÅ
			if(pBuff->data[0] == 0){
				memcpy(&pBuff->data[1],SystemInfo.pcbSn,8);
				dataLen = 9;
			}
			break;
        case DEBUG_CMD_REBOOT:	//ÖØÆôÏµÍ³
			pBuff->data[0] = 0;
            App_ProTest_SendPkt(pBuff, 1);
            osDelay(500);
            NVIC_SystemReset();
            break;
		case DEBUG_CMD_GETSN:	//»ñÈ¡×®±àºÅ
			CL_LOG("ProTest get sn req..\r\n");
			memcpy(pBuff->data,SystemInfo.stationId,8);
			dataLen = 8;
			break;
		case DEBUG_CMD_GETICODE:		//Ê¶±ðÂë
			CL_LOG("ProTest get icode req..\r\n");
			memcpy(pBuff->data,SystemInfo.idCode,8);
			dataLen = 8;
			break;
		case DEBUG_CMD_FW_UPGRADE://¹Ì¼þÉý¼¶
		{
			DEBUG_DOWN_FW_STR * fw = (void*)pBuff->data;
			App_Upgrade_Init(UPGRADE_UART,fw->package_num);
			pBuff->data[0] = 0;
		}
			break;
		case DEBUG_CMD_FW_DOWNLOAD://¹Ì¼þÏÂ·¢
		{
			if(GlobalInfo.upgradeFlag == OTA_UART)
			{
				int ret = App_Upgrade_Write(pBuff->data,pBuff->head.len - 4);
				if(ret == CL_FAIL)//
				{
					pBuff->data[0] = 1;
				}
				else if(ret == CL_OK)//Éý¼¶OK
				{
					pBuff->data[0] = 0;
					App_ProTest_SendPkt(pBuff, 1);
					App_Upgrade_Finish(0);
				}
				else if(ret == 1)
				{
					pBuff->data[0] = 0;
				}
				else//ÏÂÔØÊ§°Ü£¬ÍË³öÉý¼¶
				{
					pBuff->data[0] = 1;
					App_Upgrade_Finish(1);
				}
			}
			else
			{
				pBuff->data[0] = 1;
			}
		}
			break;
		default:
			CL_LOG("no find cmd=%x\n",pBuff->head.cmd);
			pBuff->data[0] = 1;
			break;
    }
    App_ProTest_SendPkt(pBuff, dataLen);
}

void App_ProTest_Init(void)
{
	timerHandle = xTimerCreate("timer", 1000*100, pdFALSE, (void*)0, ProTest_Timeout);
}

