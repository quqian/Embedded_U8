
#include "led.h"



/*****************************************************************************
** Function name:   	LedInit
** Descriptions:        LedInit≥ı ºªØ
** input parameters:    
** Returned value:	    None
** Author:              quqian
*****************************************************************************/
void LedInit(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
}


void RedLed(void)
{
#if 0
    static uint8_t RedledFlag = 0;

    if(0 == RedledFlag++)
    {
        RED_LIGHT_ON();
    }
    else
    {
        RedledFlag = 0;
        RED_LIGHT_OFF();
    }     
#endif
	RED_LIGHT_ON();
}
    
void GreenLed(void)
{
    static uint8_t GreenledFlag = 0;
    
    if(0 == GreenledFlag++)
    {
        GREEN_LIGHT_ON();
    }
    else
    {
        GreenledFlag = 0;
        GREEN_LIGHT_OFF();
    }                   
}

