#include "BswDrv_IC_RF433.h"
#include "includes.h"




static void timer_configuration(void);


__weak void RF433_WaringOccurred_CallBack(uint32_t addess)
{
	//northing
	UNUSED(addess);
}

#define RF733_POWER_ON()	gpio_bit_reset(GPIOC,GPIO_PIN_9)

#define TIMER_CHANNEL	TIMER_CH_2		//捕获通道
#define IC_TIMER			TIMER7		//捕获定时器


HANDLE_RF433_DATA_STATE rfState=RF433_IDLE;
uint16_t readvalue = 0;
__IO int count = 0;
uint16_t lastRecordVal = 0;
uint16_t lastRecordCount = 0;
uint8_t syncFlag=0;
uint8_t recAddrLen = 0;
uint32_t currentRfAddr=0;


void TIMER7_Channel_IRQHandler(void)
{
	int temp = 0;
	
	if(SET == timer_interrupt_flag_get(IC_TIMER,TIMER_INT_FLAG_CH2))
	{
		TIMER_CHCTL2(IC_TIMER) ^= (TIMER_IC_POLARITY_FALLING<<8);//边沿捕获反转
		
		readvalue = timer_channel_capture_value_register_read(IC_TIMER,TIMER_CHANNEL);
		
		temp = readvalue-lastRecordVal;
		if (temp < 0) 
		{
			count = 65535 + temp;
		}
		else 
		{
			count = temp;
		}
		
		switch (rfState) 
		{
			case RF433_IDLE:
				if (11550<count && count<11700) 
				{//同步码11.65ms
					if (lastRecordCount>400&&lastRecordCount<600) 
					{
						rfState = RF433_SYNC;
						syncFlag = 0;
						recAddrLen = 0;
						currentRfAddr = 0;
					}
				}
			break;
			case RF433_SYNC://接收地址码
				if (350<count&&count<600) 
				{
					if (syncFlag == 0) 
					{
						syncFlag = 1;
					} 
					else 
					{
						if (1000<lastRecordCount && lastRecordCount<1600) 
						{
							currentRfAddr |= (0<<recAddrLen);
							recAddrLen ++;
						} 
						else 
						{
							rfState = RF433_IDLE;	
						}
						syncFlag = 0; 
					}
				} 
				else if (1000<count && count<1600) 
				{
					if (syncFlag == 0) 
					{
						syncFlag = 1;
					} 
					else 
					{
						if (350<lastRecordCount && lastRecordCount<600) 
						{
							currentRfAddr |= (1<<recAddrLen);
							recAddrLen ++;
						} 
						else 
						{
							rfState = RF433_IDLE;
						}
						syncFlag = 0;
					}
				} 
				else 
				{
					rfState = RF433_IDLE;	
					recAddrLen = 0;
					break;
				}
				if (recAddrLen >= 24) 
				{
					recAddrLen = 0;
					rfState = RF433_DATA;
					recAddrLen = 0;
				}
			break;
			case RF433_DATA:
				if (350<count&&count<600) 
				{
					recAddrLen++;	
				}
				if (recAddrLen>=4) 
				{
					rfState = RF433_STOP;
				}
			break;
			case RF433_STOP:
				currentRfAddr = (uint32_t)((currentRfAddr>>4) & 0x0FFFFF);
				RF433_WaringOccurred_CallBack(currentRfAddr);
				rfState = RF433_IDLE;
			break;
			default:
				rfState = RF433_IDLE;
				break;
		}

		lastRecordVal = readvalue;
		lastRecordCount = count;

		/* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(IC_TIMER,TIMER_INT_FLAG_CH2);
    }
}

void timer_configuration(void)
{
    timer_ic_parameter_struct timer_icinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER7);

    timer_deinit(IC_TIMER);

    /* TIMER2 configuration */
    timer_initpara.prescaler         = 120-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 65535;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(IC_TIMER,&timer_initpara);

    /* TIMER2  configuration */
    /* TIMER2 CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_BOTH_EDGE;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_capture_config(IC_TIMER,TIMER_CHANNEL,&timer_icinitpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(IC_TIMER);
    /* clear  interrupt bit */
    timer_interrupt_flag_clear(IC_TIMER,TIMER_INT_FLAG_CH2);
	timer_flag_clear(IC_TIMER, TIMER_FLAG_CH2O);
    /* channel 0 interrupt enable */
    timer_interrupt_enable(IC_TIMER,TIMER_INT_CH2);
	timer_interrupt_enable(IC_TIMER,TIMER_FLAG_CH2O);
	nvic_irq_enable(TIMER7_Channel_IRQn, 3, 0);
    /* TIMER2 counter enable */
    timer_enable(IC_TIMER);
}


void BswDrv_RF433_Init(void)
{
	rfState=RF433_IDLE;
	//打开电源
	 RF733_POWER_ON();

	timer_configuration();
}





