#include "includes.h"
#include "BswDrv_GPIO.h"

static void BswDrv_GPIO_EXIT_init(uint8_t nvic_irq,uint8_t output_port,uint8_t output_pin,exti_line_enum linex);

void TIMER0_UP_IRQHandler(void)
{
	if(RESET != timer_interrupt_flag_get(TIMER0, TIMER_INT_FLAG_UP))
    {
        timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_UP);
        
		DOOR_LOW();

		timer_interrupt_disable(TIMER0, TIMER_INT_FLAG_UP);
		nvic_irq_disable(TIMER0_UP_IRQn);
		timer_flag_clear(TIMER0,TIMER_FLAG_UP);
		timer_disable(TIMER0);
    }
}



void BswDrv_GPIO_EXIT_init(uint8_t nvic_irq,uint8_t output_port,uint8_t output_pin,exti_line_enum linex)
{
	nvic_irq_enable(nvic_irq, 4U, 0U);

	gpio_exti_source_select(output_port, output_pin);

	exti_init(linex, EXTI_INTERRUPT, EXTI_TRIG_BOTH);

	exti_interrupt_flag_clear(linex);
     
}

void BswDrv_GPIO_Init (void)
{
	/* enable  clock */
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);
	//使能swd，禁止jtag
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP,ENABLE);
	
	/* WIFI模块 */
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0); 	//WL_EN
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);	//WL_SLP
	
	/* 刷卡版 */
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2); 	//PWR_EN
	
	/* RF433检测--输入捕获脚 默认 TIMER7_CH3*/
	// gpio_pin_remap_config(GPIO_TIMER2_FULL_REMAP,ENABLE); //将 TIMER2_CH2 映射到 PC8引脚  
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9); 	//433_EN
	gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);		//433_DO
	
	/* GPRS-4G模块 */
	gpio_init(GPIOA, GPIO_MODE_IPU,    GPIO_OSPEED_50MHZ, GPIO_PIN_1); 	//4GZ_RI
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);	//4G_WAKE_M
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);	//4G_PWRKEY
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);	//4G_RESET
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);	//4G_EN
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);	//4G_SLEEP
	
	/* FM1752读卡器 */
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);	//NSS
	gpio_init(GPIOB, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);		//CIRQ
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);	//CPDN
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);	//NFC_EN
	
	/* TP触摸IC*/
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);	//TP_EN
	gpio_init(GPIOC, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_4);		//TP_INT
	//外部中断配置
	BswDrv_GPIO_EXIT_init(EXTI4_IRQn,GPIO_PORT_SOURCE_GPIOC,GPIO_PIN_SOURCE_4,EXTI_4);
	
	/* LVD 掉电检测 */
	gpio_init(GPIOC, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_0);	//LVD
	//外部中断配置
	BswDrv_GPIO_EXIT_init(EXTI0_IRQn,GPIO_PORT_SOURCE_GPIOC,GPIO_PIN_SOURCE_0,EXTI_0);
	
	/* Door门禁 */
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);	//OPEN
	gpio_init(GPIOC, GPIO_MODE_IPU,    GPIO_OSPEED_50MHZ, GPIO_PIN_14);	//S_CLOSE_DET
	gpio_init(GPIOC, GPIO_MODE_IPU,    GPIO_OSPEED_50MHZ, GPIO_PIN_1);	//CLOSE_DET
	
	
	/* SC8042语音模块*/
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);	//AUDIO_EN
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);	//SC8042_RST
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);	//SC8042_DATA
	gpio_init(GPIOB, GPIO_MODE_IPD,    GPIO_OSPEED_50MHZ, GPIO_PIN_1);	//SC8042_BUSY --下拉输入
	
	/* LED */
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);	//LED_B
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);	//LED_R
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);	//LED_G

	LED_R_OFF();
	LED_G_OFF();
	LED_B_OFF();

	DOOR_LOW();
}

/**
 * 控制门和锁打开--200ms的脉冲
 * @flag-- 
 * 		TYPE_SYNC: 同步方式打开
 * 		TYPE_ASYNC：异步方式打开
 */
void BswDrv_OpenDoor(DOOR_CTRL_TYPE flag)
{
	CL_LOG("OpenDoor ctrl.\r\n");
	
	DOOR_HIGH();

	if(flag == TYPE_SYNC)//同步
	{
		osDelay(1000);
		DOOR_LOW();
	}
	else if(flag == TYPE_ASYNC)//异步
	{
		timer_interrupt_enable(TIMER0, TIMER_INT_FLAG_UP);
		nvic_irq_enable(TIMER0_UP_IRQn, 5, 0);
		timer_enable(TIMER0);
	}
}
