#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "rtc.h"
#include "includes.h"
#include "gd32f3x0_pmu.h"
#include "gd32f3x0_rcu.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


__IO uint32_t prescaler_a = 0;
__IO uint32_t prescaler_s = 0;


void rtc_show_time(void)
{
    rtc_current_time_get(&GlobalInfo.RtcData); 
}

void rtc_show_alarm(void)
{
    rtc_alarm_get(&GlobalInfo.rtc_alarm);
}

#define RTC_CLOCK_SOURCE_LXTAL
void rtc_pre_config(void)
{
    /* enable access to RTC registers in backup domain */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

#if defined (RTC_CLOCK_SOURCE_IRC40K)    
	/* enable the IRC40K oscillator */
	rcu_osci_on(RCU_IRC40K);
	/* wait till IRC40K is ready */
	rcu_osci_stab_wait(RCU_IRC40K);
	/* select the RTC clock source */
	rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);

	prescaler_s = 0x18F;
	prescaler_a = 0x63;
#elif defined (RTC_CLOCK_SOURCE_LXTAL)
	rcu_osci_on(RCU_LXTAL);
	rcu_osci_stab_wait(RCU_LXTAL);
	/* select the RTC clock source */
	rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

	prescaler_s = 0xFF;
	prescaler_a = 0x7F;
#else
    #error RTC clock source should be defined.
#endif /* RTC_CLOCK_SOURCE_IRC40K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

#if 0
/*!
    \brief      use hyperterminal to setup RTC time and alarm
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_setup(void)
{
    GlobalInfo.RtcData.rtc_factor_asyn = prescaler_a;
    GlobalInfo.RtcData.rtc_factor_syn = prescaler_s;
    GlobalInfo.RtcData.rtc_year = 0x18;
    GlobalInfo.RtcData.rtc_day_of_week = RTC_THURSDAY;
    GlobalInfo.RtcData.rtc_month = RTC_NOV;
    GlobalInfo.RtcData.rtc_date = 0x29;
    GlobalInfo.RtcData.rtc_display_format = RTC_24HOUR;
    GlobalInfo.RtcData.rtc_am_pm = RTC_AM;


    GlobalInfo.RtcData.rtc_hour = 0x16;
    GlobalInfo.RtcData.rtc_minute =0x28;
    GlobalInfo.RtcData.rtc_second = 0x00;

	SetRtcTime(&GlobalInfo.RtcData);
    RTC_BKP0 = BKP_VALUE;
#if RTC_USE_ALARM
    rtc_alarm_disable();
    GlobalInfo.rtc_alarm.rtc_alarm_mask = RTC_ALARM_DATE_MASK|RTC_ALARM_HOUR_MASK|RTC_ALARM_MINUTE_MASK;
    GlobalInfo.rtc_alarm.rtc_weekday_or_date = RTC_ALARM_DATE_SELECTED;
    GlobalInfo.rtc_alarm.rtc_alarm_day = 0x31;
    GlobalInfo.rtc_alarm.rtc_am_pm = RTC_AM;
    GlobalInfo.rtc_alarm.rtc_alarm_hour = 03;
    GlobalInfo.rtc_alarm.rtc_alarm_minute = 01;
    GlobalInfo.rtc_alarm.rtc_alarm_second = 04;
    rtc_alarm_config(&GlobalInfo.rtc_alarm);
    rtc_show_time();
    rtc_show_alarm(); 
    rtc_interrupt_enable(RTC_INT_ALARM);  
    rtc_alarm_enable();    
#endif
}
#endif

void RtcInit(void)
{
    rtc_pre_config();
	CL_LOG("初始化 RTC\n");
#if 0
	if (BKP_VALUE != RTC_BKP0)
	{
        rtc_setup(); 
	//	SetRtcCount(1543629490);
    }
	else
	{
		rtc_show_time();
		CL_LOG("显示RTC时间\n");
	#if RTC_USE_ALARM
        rtc_flag_clear(RTC_STAT_ALRM0F);
//        exti_flag_clear(EXTI_17);
        rtc_show_alarm();
	#endif
    }
#endif
}




/*******************************************************
函数原型:   int LinuxTickToDay(time_t timestamp, uint8_t *pDay)
函数参数:   time_t timestamp:Linux时间戳
            uint8_t *pDay：Linux时间戳转换后的时间
函数功能:   将Linux时间戳转换为当前的时间
返回值  :   无
********************************************************/
//char gStrfTime[32];
int LinuxTickToDay(time_t timestamp, uint8_t *pDay)
{
    struct tm *time_now;

	time_now = localtime(&timestamp);
    pDay[0] = time_now->tm_wday;
    pDay[1] = time_now->tm_year-100;
    pDay[2] = time_now->tm_mon+1;
    pDay[3] = time_now->tm_mday;
    pDay[4] = time_now->tm_hour + 8;
    pDay[5] = time_now->tm_min;
    pDay[6] = time_now->tm_sec;
    if(24 < pDay[4])
    {
        pDay[4] = pDay[4] - 24;
        pDay[3] = pDay[3] + 1;
    }
    if(60 < pDay[5])
    {
        pDay[5] = pDay[5] - 60;
        pDay[4] = pDay[4] + 1;
    }
    if(60 < pDay[6])
    {
        pDay[6] = pDay[6] - 60;
        pDay[5] = pDay[5] + 1;
    }
//	strftime(gStrfTime, sizeof(gStrfTime), "%D%T", time_now);
//	CL_LOG("pDay[0][%d], pDay[1][%d], pDay[2][%d], pDay[3][%d], pDay[4][%d], pDay[5][%d], pDay[6][%d], \n", pDay[0], pDay[1], pDay[2], pDay[3], pDay[4], pDay[5], pDay[6]);

    return CL_OK;
}

//time_t DayToTimeStamp(char* str)
//{
//#if 0
//	struct tm time_now;
//    time_t TimeStamp;
//	
//	strptime("20131120","%Y%m%d", &time_now);
//	TimeStamp = mktime(&time_now);
//	
//    return TimeStamp;
//#endif
//}

/*******************************************************
函数原型:   char* GetCurrentTime(void)
函数参数:   无
函数功能:   得到当前的时间
返回值  :   char *
********************************************************/
static char gTimeStr[32];
char* GetCurrentTime(void)
{
#if 0
    uint32_t time;
    uint8_t day[8];
	
//    time = rtc_counter_get();
    LinuxTickToDay(time,day);
    sprintf(gTimeStr,"%4d%02u%02u %02u:%02u:%02u", day[1]+100+1900,day[2], day[3], day[4], day[5], day[6]);
#else
	rtc_current_time_get(&GlobalInfo.RtcData);
	sprintf(gTimeStr,"%2x%02x%02x %02x:%02x:%02x", GlobalInfo.RtcData.rtc_year, GlobalInfo.RtcData.rtc_month, 
		GlobalInfo.RtcData.rtc_date, GlobalInfo.RtcData.rtc_hour, GlobalInfo.RtcData.rtc_minute, GlobalInfo.RtcData.rtc_second);
#endif
	return gTimeStr;
}

uint8_t HEX2BCD(uint8_t bcd_data)  
{   
    uint8_t temp; 

    temp = (bcd_data / 10 * 16 + bcd_data % 10);  
	
	return temp;   
}   

uint8_t BCD2HEX(uint8_t hex_data)  
{   
    uint8_t temp;   
	
    temp = (hex_data / 16 * 10 + hex_data % 16);   
	
    return temp;   
} 

void SetRtcTime(rtc_parameter_struct* rtc_initpara_struct)
{
	rtc_init(rtc_initpara_struct);
}

/*******************************************************
函数原型:   void SetRtcCount(time_t timestamp)
函数参数:   time_t timestamp：Linux时间戳
函数功能:   将Linux时间戳转换为当前的时间并设置到RTC寄存器
返回值  :
********************************************************/
void SetRtcCount(time_t timestamp)
{
	uint8_t day[8];
	
    LinuxTickToDay(timestamp, day);
	GlobalInfo.RtcData.rtc_year		 	= HEX2BCD(day[1]);
	GlobalInfo.RtcData.rtc_month		= HEX2BCD(day[2]);
	GlobalInfo.RtcData.rtc_date		 	= HEX2BCD(day[3]);
	GlobalInfo.RtcData.rtc_day_of_week 	= HEX2BCD(day[0]);
	GlobalInfo.RtcData.rtc_hour		 	= HEX2BCD(day[4]);
	GlobalInfo.RtcData.rtc_minute 	 	= HEX2BCD(day[5]);
	GlobalInfo.RtcData.rtc_second 	 	= HEX2BCD(day[6]);
	GlobalInfo.RtcData.rtc_factor_asyn 	= prescaler_a;
	GlobalInfo.RtcData.rtc_factor_syn 	= prescaler_s;
	GlobalInfo.RtcData.rtc_am_pm 		= RTC_AM;		//0:AM  !0:PM
	GlobalInfo.RtcData.rtc_display_format = RTC_24HOUR;

	SetRtcTime(&GlobalInfo.RtcData);
//	RTC_BKP0 = BKP_VALUE;
}

int64_t GetRtcTimeStamp(void)
{
    struct tm TimeTickss;
    rtc_parameter_struct RtcData = {0,};
    
	rtc_current_time_get(&RtcData);
    memset(&TimeTickss,0,sizeof(struct tm));
    TimeTickss.tm_year =  BCD2HEX(RtcData.rtc_year);
    TimeTickss.tm_mon = BCD2HEX(RtcData.rtc_month);
    TimeTickss.tm_mday = BCD2HEX(RtcData.rtc_date);
    TimeTickss.tm_wday = BCD2HEX(RtcData.rtc_day_of_week);
    TimeTickss.tm_hour = BCD2HEX(RtcData.rtc_hour);
    TimeTickss.tm_min = BCD2HEX(RtcData.rtc_minute);
    TimeTickss.tm_sec = BCD2HEX(RtcData.rtc_second);
    
//    printf("\naaaaa[%d,%d,%d,%d,%d,%d] \n", TimeTickss.tm_year, TimeTickss.tm_mon, 
 //   TimeTickss.tm_mday, TimeTickss.tm_hour, TimeTickss.tm_min, TimeTickss.tm_sec);
    
    return mktime(&TimeTickss);
}




