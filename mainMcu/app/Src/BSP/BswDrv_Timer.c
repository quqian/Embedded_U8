#include "includes.h"
#include "BswDrv_Timer.h"


static void TimerConfig(void);




/*****************************************************************************
** Function name:   	TimerConfig
** Descriptions:        configure the TIMER peripheral
** input parameters:    
** Returned value:	    None
** Author:             
*****************************************************************************/
void TimerConfig(void)
{
    /* TIMER0CLK = SystemCoreClock / 120 = 1MHz */
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);
	
    /* TIMER0 configuration */
    timer_initpara.prescaler         = 12000-1;	//100us
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 10000-1;	//1s
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

	// timer_interrupt_enable(TIMER0, TIMER_INT_FLAG_UP);
	// nvic_irq_enable(TIMER0_UP_IRQn, 5, 0);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);

    timer_flag_clear(TIMER0,TIMER_FLAG_UP);
    // timer_enable(TIMER0);
}


#if 0

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer1_config(void)//调试 pwm输出 
{
    /* -----------------------------------------------------------------------
    TIMER1 configuration: generate 3 PWM signals with 3 different duty cycles:
    TIMER1CLK = SystemCoreClock / 120 = 1MHz

    TIMER1 channel0 duty cycle = (4000/ 16000)* 100  = 25%
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 120-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* CH0,CH1 and CH2 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER1,TIMER_CH_0,&timer_ocintpara);


    /* CH0 configuration in PWM mode0,duty cycle 25% */
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,399);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    /* auto-reload preload enable */
    timer_enable(TIMER1);
}

#endif

/**
 * 通用定时器5 --用于延时
 */ 
void Delay_Timer_Init(void)
 {
     timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER5);
    timer_deinit(TIMER5);
	
    /* TIMER5 configuration */
    timer_initpara.prescaler         = 120-1;	//1MHz--1us
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 100-1;	    //100us
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER5, &timer_initpara);

    timer_auto_reload_shadow_enable(TIMER5);

    timer_flag_clear(TIMER5,TIMER_FLAG_UP);
 }

void BswDrv_Timer_Init(void)
{
	TimerConfig();

    Delay_Timer_Init();	
}

