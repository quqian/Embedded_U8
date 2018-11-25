#include "includes.h"
#include "gd32f3x0.h"
#include "systick.h"
//#include "FreeRTOSConfig.h"
//#include "portmacro.h"
//#include "task.h"

#define TICK_RATE_HZ				(1000U)

static uint32_t DelayCount;


void SysTick_Handler(void)
{	
    //if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
    //{
    //    xPortSysTickHandler();	
    //}
    //gpio_bit_toggle(GPIOA, GPIO_PIN_8);
}

void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / TICK_RATE_HZ))
	{
        /* capture error */
        while (1);
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void delay_1ms(uint32_t count)
{
    DelayCount = count;

    while(0U != DelayCount);
}

void delay_decrement(void)
{
    if (0U != DelayCount)
	{
        DelayCount--;
    }
}







static uint8_t  fac_us=0;							//us延时倍乘数			   
//static uint16_t fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数

void SystickInit(void)
{
	uint32_t reload;
	
	systick_clksource_set(SYSTICK_CLKSOURCE_HCLK);//选择外部时钟  HCLK
	fac_us = SystemCoreClock / 1000000;				//不论是否使用OS,fac_us都需要使用
	reload = SystemCoreClock / 1000000;				//每秒钟的计数次数 单位为M  
	reload *= 1000000 / TICK_RATE_HZ;			//根据configTICK_RATE_HZ设定溢出时间
												//reload为24位寄存器,最大值:16777216,在72M下,约合0.233s左右	
//	fac_ms = 1000 / configTICK_RATE_HZ;				//代表OS可以延时的最少单位	   

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD = reload; 						//每1/configTICK_RATE_HZ秒中断一次	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    
}


void DelayUs(uint32_t us)
{
	uint32_t ticks;
	uint32_t told;
	uint32_t tnow;
	uint32_t tcnt = 0;
	uint32_t reload = SysTick->LOAD;				//LOAD的值	    
	
	ticks = us * fac_us; 						//需要的节拍数 
	told = SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{
			if(tnow < told)
			{
				tcnt += told - tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			}
			else
			{
				tcnt += reload - tnow + told;
			}
			told = tnow;
			if(tcnt >= ticks)
			{
				break;			//时间超过/等于要延迟的时间,则退出.
			}
		}  
	}									    
}  

void DelayMsWithNoneOs(uint32_t nms)
{
	uint32_t i;
	
	for(i = 0; i < nms; i++) 
	{
		DelayUs(1000);
	}

	return;
}



