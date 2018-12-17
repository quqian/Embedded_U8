#ifndef __RTC_H__
#define __RTC_H__

#include "includes.h"


void GetRtcTime(void* pRTCTime);
void SetRtcCount(time_t timestamp);
time_t GetTimeStamp(void);


void BswDrv_RtcInit(void);

#endif


