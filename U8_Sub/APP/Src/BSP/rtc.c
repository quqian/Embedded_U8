#include "rtc.h"
#include "includes.h"


#if 0
rtc_parameter_struct   rtc_initpara;
__IO uint32_t prescaler_a = 0;
__IO uint32_t prescaler_s = 0;

void rtc_setup(void)
{
    rtc_initpara.rtc_factor_asyn = prescaler_a;
    rtc_initpara.rtc_factor_syn = prescaler_s;
    rtc_initpara.rtc_year = 0x16;
    rtc_initpara.rtc_day_of_week = RTC_SATURDAY;
    rtc_initpara.rtc_month = RTC_APR;
    rtc_initpara.rtc_date = 0x30;
    rtc_initpara.rtc_display_format = RTC_24HOUR;
    rtc_initpara.rtc_am_pm = RTC_AM;

    /* current time input */
    printf("=======Configure RTC Time========\n\r");
	rtc_initpara.rtc_hour = 5;
    printf("  please input minute:\n\r");
	rtc_initpara.rtc_minute = 5;
    printf("  please input second:\n\r");
	rtc_initpara.rtc_second = 5;

    /* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara))
	{    
        printf("** RTC time configuration failed! **\n\r");
    }
	else
	{
        printf("** RTC time configuration success! **\n\r");
        rtc_show_time();
        RTC_BKP0 = 0x32f0;
    }   
}
#endif

void RtcInit(void)
{
    /* enable PMU and BKPI clocks */
//    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);
    /* allow access to BKP domain */
    pmu_backup_write_enable();

	/* reset BKP domain register*/
    rcu_bkp_reset_enable();
    rcu_bkp_reset_disable();
	
    rcu_osci_on(RCU_IRC40K);         //使能内部低速时钟
    rcu_osci_stab_wait(RCU_IRC40K);
    rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
//	RTC_BKP0 = 0x32f0;
    /* wait until last write operation on RTC registers has finished */
 //   rtc_lwoff_wait();
    /* set RTC prescaler: set RTC period to 1s */
 //   rtc_prescaler_set(32767);
    /* wait until last write operation on RTC registers has finished */
//    rtc_lwoff_wait();
}




/*******************************************************
函数原型:   int LinuxTickToDay(time_t timestamp, uint8_t *pDay)
函数参数:   time_t timestamp:Linux时间戳
            uint8_t *pDay：Linux时间戳转换后的时间
函数功能:   将Linux时间戳转换为当前的时间
返回值  :   无
********************************************************/
int LinuxTickToDay(time_t timestamp, uint8_t *pDay)
{
    struct tm *time_now;

	time_now = localtime(&timestamp);
    pDay[0] = time_now->tm_wday;
    pDay[1] = time_now->tm_year-100;
    pDay[2] = time_now->tm_mon+1;
    pDay[3] = time_now->tm_mday;
    pDay[4] = time_now->tm_hour+8;
    pDay[5] = time_now->tm_min;
    pDay[6] = time_now->tm_sec;
    return CL_OK;
}



/*******************************************************
函数原型:   char* GetCurrentTime(void)
函数参数:   无
函数功能:   得到当前的时间
返回值  :   char *
********************************************************/
static char gTimeStr[32];
char* GetCurrentTime(void)
{
    uint32_t time;
    uint8_t day[8];
	
    time = rtc_counter_get();
    LinuxTickToDay(time,day);
    sprintf(gTimeStr,"%4d%02u%02u %02u:%02u:%02u", day[1]+100+1900,day[2], day[3], day[4], day[5], day[6]);
	
    return gTimeStr;
}


void GetRtcTime(void* pRTCTime)
{
    uint32_t time = 0;
    time = rtc_counter_get();
    LinuxTickToDay(time,pRTCTime);
}



/*******************************************************
函数原型:   void SetRtcCount(time_t timestamp)
函数参数:   time_t timestamp：Linux时间戳
函数功能:   将Linux时间戳转换为当前的时间并设置到RTC寄存器
返回值  :
********************************************************/
void SetRtcCount(time_t timestamp)
{
    rtc_counter_set(timestamp);
}

/*******************************************************
函数原型:   time_t GetTimeStamp(void)
函数参数:   无
函数功能:   获取当前的时间戳
返回值  :   时间戳
********************************************************/
time_t GetTimeStamp(void)
{
	return rtc_counter_get();
}



