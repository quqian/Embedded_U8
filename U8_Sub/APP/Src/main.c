/*****************************************************************************
** @Author: quqian  
** @Date: 2018-11-15 14:20:14 
** @File: main.c
** @MCU: GD32F330CBT6   
** @MCU max Speed: 84M
** @MCU Flash: 128K
** @MCU RAM: 16K
** @MCU Package: LQFP48
** @Last Modified by: quqian
** @Last Modified time: 2018-12-10 14:20:14 
*****************************************************************************/
#include "includes.h"
#include "gd32f3x0.h"
#include "systick.h"
#include <stdio.h>
//#include <task.h>
//#include "wifi.h"
#include "rtc.h"
#include "system.h"
#include "usart.h"
#include "flash.h"
#include "flash_usr.h"
#include "usart.h"
#include "spi.h"
#include "NFCard.h"
#include "GPRS_4G.h"
#include "gd32f3x0_libopt.h"
#include "system.h"
#include "sc8042b.h"
#include "fm175xx.h"
#include "NFCard.h"
#include "sim_uart.h"
#include "ComProto.h"
#include "led.h"





void BspInit(void)
{
//    StartDelay();
    LedInit();
    WatchDogInit();             //看门狗初始化
    FeedWatchDog();
	SystickInit();
	SimUartInit();
	RtcInit();
    DelayMsWithNoneOs(2000);
	UsartInit();                //串口初始化
#if USE_TIMER1
    TimerConfig(1, 1000);
#endif
    printf("\n\n\n===========================================================\n");
    CL_LOG("设备启动\n");
    SystemResetRecord();
    printf("UsartInit OK!\r\n");
    SC8042B_Init();
	FM175XX_Config();

//语音提示    
    PlayVoice(VOIC_WELCOME);            //欢迎使用
    DelayMsWithNoneOs(900);
//    PlayVoice(VOIC_SHARE_CHARGE);       //共享充电
//    DelayMsWithNoneOs(900);
    printf("\r\n BspInit ok\n");
    printf("fw_version = %d, subVersion1 = %d, subVersion2 = %d.\n", (uint8_t)FW_VERSION, (uint8_t)FW_VERSION_SUB1, (uint8_t)FW_VERSION_SUB2);
    printf("\n 文件编译时间, 月日年 %s 时分秒%s \n", __DATE__, __TIME__);
    printf("\n===========================================================\n");
}


int main(void)
{
//	time_t t;
	uint32_t NFCardTicks = GetTimeTicks();
	uint32_t HeartBeatTicks = NFCardTicks;
	uint32_t ShakeHandTicks = NFCardTicks;
    uint32_t RedLedTicks = NFCardTicks;
	uint32_t GreenLedTicks = NFCardTicks;
	uint32_t TimeFlagTicks = GetTimeTicks();
	

	nvic_vector_table_set(FLASH_BASE, BOOT_SIZE);        	//设置Flash地址偏移
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);		//设置系统中断优先级分组4	
	
    BspInit();
#if 0
	GlobalInfo.RtcData.rtc_year		 	= 0x18;
	GlobalInfo.RtcData.rtc_month		= RTC_NOV ;
	GlobalInfo.RtcData.rtc_date		 	= 0x29 ;
	GlobalInfo.RtcData.rtc_day_of_week 	= RTC_THURSDAY ;
	GlobalInfo.RtcData.rtc_hour		 = 0x16;
	GlobalInfo.RtcData.rtc_minute 	 = 0x28;
	GlobalInfo.RtcData.rtc_second 	 = 0x00;
	GlobalInfo.RtcData.rtc_factor_asyn = prescaler_a;
	GlobalInfo.RtcData.rtc_factor_syn = prescaler_s;
	GlobalInfo.RtcData.rtc_am_pm = RTC_AM;		//0:AM  !0:PM
	GlobalInfo.RtcData.rtc_display_format = RTC_24HOUR;

	SetRtcTime(&GlobalInfo.RtcData);
#else
	SetRtcCount(1543547571);
//	t = 1543484295;
//	gmtime(&t);
#endif
	while(1)
    {
        FeedWatchDog();
		//DelayMsWithNoneOs(50);
		TimeFlagTicks = GetTimeTicks();
        {
        #if 1
            if(((RedLedTicks + 5000) <= TimeFlagTicks) || (RedLedTicks > TimeFlagTicks))
            {
               // CL_LOG("SystemCoreClock[%d]\n", SystemCoreClock);
                RedLedTicks = TimeFlagTicks;
                RedLed();
                
				{
					rtc_current_time_get(&GlobalInfo.RtcData);
                    printf("\n\n\n");
					CL_LOG("rtc_year[%#x]\n", GlobalInfo.RtcData.rtc_year);
					CL_LOG("rtc_month[%#x]\n", GlobalInfo.RtcData.rtc_month);
					CL_LOG("rtc_date[%#x]\n", GlobalInfo.RtcData.rtc_date);
					CL_LOG("rtc_day_of_week[%#x]\n", GlobalInfo.RtcData.rtc_day_of_week);
					CL_LOG("rtc_hour[%#x]\n", GlobalInfo.RtcData.rtc_hour);
					CL_LOG("rtc_minute[%#x]\n", GlobalInfo.RtcData.rtc_minute);
					CL_LOG("rtc_second[%#x]\n", GlobalInfo.RtcData.rtc_second);
					CL_LOG("rtc_am_pm[%#x]\n", GlobalInfo.RtcData.rtc_am_pm);
				}
            }
			
			if(((GreenLedTicks + 500) <= GetTimeTicks()) || (GreenLedTicks > GetTimeTicks()))
            {
                GreenLedTicks = GetTimeTicks();
                GreenLed();
			//	CL_LOG("SystemCoreClock[%d]\n", SystemCoreClock);
            }
	
            
			if(((NFCardTicks + 500) <= TimeFlagTicks) || (NFCardTicks > TimeFlagTicks))
            {
                NFCardTicks = TimeFlagTicks;
				if(GlobalInfo.UpgradeFlag != 0xa5)
				{
					if(CL_OK == BswDrv_FM175XX_SetPowerDown(0)) 	//退出睡眠
					{
						NFCardTask();
					}
				}
                BswDrv_FM175XX_SetPowerDown(1);			//进入睡眠
                FeedWatchDog();
            }
            
			if(0xa5 != GlobalInfo.ShakeHandState)
			{
				if(((ShakeHandTicks + 5000) <= TimeFlagTicks) || (ShakeHandTicks > TimeFlagTicks))
				{
                    FeedWatchDog();
                    ShakeHandTicks = TimeFlagTicks;
					BasicInfoShakeHand();
				}
			}
            DebugRecvProc();
            ComRecvMainBoardData();
			if(((HeartBeatTicks + 60000) <= TimeFlagTicks) || (HeartBeatTicks > TimeFlagTicks))
			{
                FeedWatchDog();
                HeartBeatTicks = TimeFlagTicks;
				BasicInfoHeartBeat();
		//		CL_LOG("SystemCoreClock[%d]\n", SystemCoreClock);
			}
		#endif
        }
    }
}





