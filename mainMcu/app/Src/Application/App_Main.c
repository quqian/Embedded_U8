#include "includes.h"
#include "BswSrv_System.h"
#include "BswDrv_Rtc.h"
#include "BswDrv_GPIO.h"
#include "BswDrv_Debug.h"
#include "BswDrv_sc8042b.h"
#include "BswSrv_RF433.h"
#include "App_Main.h" 
#include "App_NetProto.h"
#include "App_BlueProto.h"
#include "BswDrv_Sys_Flash.h"
#include "App_ProductTest.h"
#include "BswSrv_Key.h"
#include "BswSrv_WG215.h"
#include "BswSrv_FlashUsr.h"
#include "BswDrv_Watchdog.h"
#include "App_ProductTest.h"


/**
 * 按键回调函数
 */ 
void BswSrv_Key_Values(uint8_t key)
{
	if(GlobalInfo.isTest)
	{
		App_ProTest_SendKeyInfo(key);
	}
	CL_LOG("Key touch,values=%d \r\n",key);
}

/**
 *读卡器读卡回调函数
 * direction：3--进  4-出
 */
void App_ReadCard_CallBack(uint8_t direction,uint8_t *cardId)
{
	CL_LOG("cardId=%s \r\n",cardId);
	
	if(GlobalInfo.isLogin)
	{
		if(cardAuthorMsg.flag == 0)
		{
			Sc8042bSpeech(VOIC_READING_CARD);
			cardAuthorMsg.flag = 1;
			cardAuthorMsg.repeat = 2;
			cardAuthorMsg.startTime = 0;
			cardAuthorMsg.sn = GetFrameStrSn();
			cardAuthorMsg.direction = direction;
			memcpy(cardAuthorMsg.cardId,cardId,16);
		}else{
			CL_LOG("card is authoring.\r\n");
			Sc8042bSpeech(VOIC_CARD);
		}
	}else{
		Sc8042bSpeech(VOIC_CARD);
	}

	if(GlobalInfo.isTest)
	{
		App_ProTest_SendCardInfo(cardId,direction);
	}
}

/**
 *烟雾报警事件超时
 */
void BswSrv_RF433_EventTimeout(RF_Unit_Typedef *dev)
{
	CL_LOG("check rf433 timeout:num=%d address=%X \r\n",dev->num,dev->address);
	App_NetProto_SendEventNotice(0,EVENT_SMOKE_WARING,dev->num,dev->address,2,NULL);
}


void App_Test(void)
{
	// uint8_t buf[64] = {0,};
	// BswDrv_SysFlashErase(OtherInfoAddr,1);
	// BswDrv_SysFlashWrite(OtherInfoAddr,"a1",2);
	// FlashRead(OtherInfoAddr,buf,64);
	// CL_LOG("read buf=%s \r\n",buf);		
	// SetRtcCount(1543913133);
	// CL_LOG("getRTC----------%d\r\n",GetRtcCount());
}


void App_MainTask(void)
{
	uint8_t first = 1;
	time_t tick = 0;
	uint32_t second = 0;
	
	RF_Unit_Typedef rf433Dev;

	osDelay(1000);
	
	Sc8042bSpeech(VOIC_WELCOME);
	
	App_Test();
	
	while(1)
	{
		osDelay(80);

		//低电压检测..
		if(READ_LVD_IN() == 0)
		{
			//todo
		}
		
		//读取按键状态
		BswSrv_Key_Loop();

		if(GlobalInfo.upgradeFlag != OTA_NONE)
		{
			//蓝牙和串口升级，3分钟OTA不成功,退出下载
			if((GlobalInfo.upgradeFlag == OTA_BLUE || GlobalInfo.upgradeFlag == OTA_UART) && 
				(uint32_t)(osGetTimes() - GlobalInfo.upgradeTime) >= (1000*180))
			{
				CL_LOG("upgrade timeout,exit \r\n");
				GlobalInfo.upgradeFlag = OTA_NONE;
			}
			//指示灯
			LED_G_TOGGLE();
		}

		//发送刷卡鉴权
		if(cardAuthorMsg.flag == 1 && (osGetTimes() - cardAuthorMsg.startTime) >= 3000)
		{
			cardAuthorMsg.startTime = osGetTimes();
			App_NetProto_SendCardAuthReq(cardAuthorMsg.cardId,cardAuthorMsg.direction,cardAuthorMsg.sn);
			if(cardAuthorMsg.repeat-- <= 1)
			{
				memset(&cardAuthorMsg,0,sizeof(cardAuthorMsg));
				PlayWaringVoice();
			}
		}
		
		if(osGetTimes() - tick >= 1000)
		{
			tick = osGetTimes();

			//喂狗
			BswDrv_FeedWatchDog();

			//读取门开关状态
			if(READ_DOOR_STATE() == 1)
			{
				GlobalInfo.doorState = 1;
			}else
			{
				GlobalInfo.doorState = 0;
			}
			if(GlobalInfo.doorStatePre != GlobalInfo.doorState)
			{
				CL_LOG("门状态变化，state=%d \r\n",GlobalInfo.doorState);
				GlobalInfo.doorStatePre = GlobalInfo.doorState;
				if(GlobalInfo.isLogin)
				{
					App_NetProto_SendHeartBeat();
				}
			}
			//轮询报警超时事件
			if(second % 5 == 0)
			{
				BswSrv_RF433_Loop_EventTimeout();
			}
		
			//获取报警设备信息
			if(CL_OK == BswSrv_RF433_GetWaringDevice(&rf433Dev))
			{
				CL_LOG("check rf433 waring:addr=%X \r\n",rf433Dev.address);
				App_NetProto_SendEventNotice(0,EVENT_SMOKE_WARING,rf433Dev.num,rf433Dev.address,1,NULL);
			}

			//测试模式下-检测烟感
			if(GlobalInfo.isTest)
			{
				if(isCheckRF433 == 1)
				{
					App_ProTest_SendRF433Rst(0,readRF433Addr);
					isCheckRF433 = 0;
				}
				else if(isCheckRF433 == 2)
				{
					App_ProTest_SendRF433Rst(1,readRF433Addr);
					isCheckRF433 = 0;
				}
			}

			//网络处理
			if(GlobalInfo.is_socket_0_ok && GlobalInfo.upgradeFlag != OTA_NET)
			{
				/*注册*/
				if(GlobalInfo.isRegister == 0)
				{
					if(first){
						App_NetProto_SendRegister();
						first = 0;
					}else{
						if(second % 60 == 0){
							App_NetProto_SendRegister();
						}
					}
				}
				else
				{
					/*登陆*/
					if(GlobalInfo.isLogin == 0)
					{
						if(first){
							App_NetProto_SendStartUpNotice(1);
							first = 0;
						}else{
							if(second % 60 == 0){
								App_NetProto_SendStartUpNotice(1);
							}
						}
					}
					else
					{
						LED_G_TOGGLE();
						/*心跳*/
						if(second % 60 == 0){
							App_NetProto_SendHeartBeat();
						}
						
						//请求绑定设备 todo
					}
				}
			}else{
				first = 1;
				LED_G_OFF();
			}

			//蓝牙处理
			if(GlobalInfo.isBlueConnect && GlobalInfo.upgradeFlag != OTA_BLUE)
			{
				//如果蓝牙已经登陆--定时发送心跳
				if(GlobalInfo.isBlueLogin && second%10 == 0)
				{
					App_Blue_SendHeartBat();
				}
				if(osGetTimes() - GlobalInfo.blueLastOnlineTime > 1000*30)
				{
					CL_LOG("check blue connect timeout.\n");
					//断开蓝牙连接
					BswSrv_Blue_Disconnent();
					
					GlobalInfo.isBlueConnect = 0;
					
					if(GlobalInfo.isBlueLogin)
					{
						GlobalInfo.isBlueLogin = 0;
						Sc8042bSpeech(VOIC_BLUETOOTH_OFFLINE);
					}
				}
			}
			second++;
		}
	}
}
