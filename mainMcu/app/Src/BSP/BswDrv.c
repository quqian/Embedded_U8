#include "includes.h"
#include "BswDrv.h"
#include "BswDrv_Rtc.h"
#include "BswDrv_Usart.h"
#include "BswDrv_GPIO.h"
#include "BswDrv_Timer.h"
#include "BswDrv_SPI.h"
#include "BswDrv_IIC.h"
#include "BswDrv_IC_RF433.h"
#include "BswDrv_Sys_Flash.h"
#include "BswDrv_Delay.h"
#include "BswDrv_sc8042b.h"
#include "BswDrv_Adc.h"
#include "BswDrv_Watchdog.h"


void BswDrv_Init(void)
{	
	/*设置系统中断优先级分组4	*/
	nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
	
	/*GPIO初始化*/
	BswDrv_GPIO_Init(); 
	
	/*串口初始化*/
	BswDrv_UsartInit();

	/*RCT初始化*/
	BswDrv_RtcInit();
	
	/* SPI初始化 */
	BswDrv_SPI_Init();

	/* IIC初始化 */
	BswDrv_IIC_init();
	
	/*片内flash初始化*/
	BswDrv_SysFlashInit();
	
	/*定时器初始化 */
	BswDrv_Timer_Init();

	/*ACD初始化*/
	BswDrv_ADC_Init();

	/*433模块初始化*/
	BswDrv_RF433_Init();

	/*语音模块初始化*/
	BswDrv_SC8042B_Init();

	BswDrv_WatchDogInit();
}

