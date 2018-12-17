#include "gd32f30x.h"
#include "BswDrv_Delay.h"


void BswDrv_HwDelay_100us(void)
{
	// timer_autoreload_value_config(TIMER5,0);
	timer_counter_value_config(TIMER5,0);
	timer_enable(TIMER5);

	while(timer_flag_get(TIMER5,TIMER_FLAG_UP) == RESET)
	{
		;
	}
	timer_flag_clear(TIMER5,TIMER_FLAG_UP);
	
	timer_disable(TIMER5);
}



void BswDrv_SoftDelay_ms(uint16_t mSec)
{
	while (mSec--)
	{
		BswDrv_SoftDelay_us(1000);
	}
}


void BswDrv_SoftDelay_us(uint16_t count)
{
	__IO uint32_t i = 0;
	__IO uint8_t j = 0;

	for(i=0; i<count; i++) {
	
		for(j=0; j<8; j++);
	}
}

