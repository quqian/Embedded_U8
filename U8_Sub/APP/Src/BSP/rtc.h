#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>
#include "includes.h"
#include "gd32f3x0_rtc.h"


#define BKP_VALUE    		0x32F0
#define RTC_USE_ALARM		0


extern __IO uint32_t prescaler_a;
extern __IO uint32_t prescaler_s;

extern void RtcInit(void);
extern void GetRtcTime(void* pRTCTime);
extern void SetRtcCount(time_t timestamp);
extern void SetRtcTime(rtc_parameter_struct* rtc_initpara_struct);
extern time_t GetTimeStamp(void);
extern char* GetCurrentTime(void);
extern uint8_t BCD2HEX(uint8_t hex_data);
extern int64_t GetRtcTimeStamp(void);

#endif


