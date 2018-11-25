#include "GPRS_4G.h"
#include "system.h"
#include "usart.h"
#include "rtc.h"


void GPRS_4G_Task(void)
{
    uint32_t TimeTicks = 0;
    
    while(1)
    {
		while(1)
		{
	        vTaskDelay(100);
            
            if(10 <= (GetRtcCount() - TimeTicks))
			{
				TimeTicks = GetRtcCount();
                //CL_LOG("GPRS_4G_Task! \r\n");
            }
			FeedWatchDog();
		}
	}
}


