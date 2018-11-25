#ifndef __RTC_H__
#define __RTC_H__

#include "includes.h"




extern void RtcInit(void);
extern void GetRtcTime(void* pRTCTime);
extern void SetRtcCount(time_t timestamp);
extern time_t GetTimeStamp(void);



#endif


