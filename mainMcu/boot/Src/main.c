#include "includes.h"
#include "upgrade.h"

#define DEBUG_COM                        UART4


extern SYS_UPDATE_INFO_T updateInfo;

typedef  void (*pFunction)(void);


int fputc(int ch, FILE *f)
{
    usart_data_transmit(DEBUG_COM, (uint8_t)ch);
    
    while(RESET == usart_flag_get(DEBUG_COM, USART_FLAG_TBE));
    
    return ch;
}



void SoftDelay_ms(uint16_t mSec)
{
	uint32_t	i;
	while (mSec--)
	{
		for (i=0; i<3300; i++)
		{
			__NOP();
			__NOP();
			__NOP();
			__NOP();
		}
	}
}


/**
  * @brief  控制程序跳转到指定位置开始执行 。
  * @param  Addr 程序执行地址。
  * @retval 程序跳转状态。
  */
uint8_t JumpToApplication(uint32_t Addr)
{
	pFunction Jump_To_Application;
	__IO uint32_t JumpAddress; 
	/* Test if user code is programmed starting from address "ApplicationAddress" */
	if (((*(__IO uint32_t*)Addr) & 0x2FFE0000 ) == 0x20000000)
	{ 
	  /* Jump to user application */
	  JumpAddress = *(__IO uint32_t*) (Addr + 4);
	  Jump_To_Application = (pFunction) JumpAddress; 
	  /* Initialize user application's Stack Pointer */
	  __set_MSP(*(__IO uint32_t*) Addr);

	  Jump_To_Application();
	}
	return 1;
}

void Debug_Uart_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_UART4);
	
	gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_12);
    gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_MAX, GPIO_PIN_2);
	
	usart_deinit(DEBUG_COM);
    usart_baudrate_set(DEBUG_COM, 115200);            
    usart_parity_config(DEBUG_COM, USART_PM_NONE);
    usart_word_length_set(DEBUG_COM, USART_WL_8BIT);
    usart_stop_bit_set(DEBUG_COM, USART_STB_1BIT);
	
	usart_receive_config(DEBUG_COM, USART_RECEIVE_ENABLE);     
    usart_transmit_config(DEBUG_COM, USART_TRANSMIT_ENABLE);
	
	usart_enable(DEBUG_COM);
}


int main(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);//设置系统中断优先级分组4	
    
	Debug_Uart_Init();

	printf("\n|=========================U8 Bootload==========================|\n");
	printf("|Verson:%d                                                      |\n",FW_VERSION);
	printf("|Date:%s                                              |\n",__DATE__);
	printf("|==============================================================|\n");
    
	
	BswDrv_SysFlashRead(SysUpInfoAddr,(void*)&updateInfo,sizeof(updateInfo));
	printf("headFlag=%X upgradeFlag = %d \r\n",updateInfo.headFlag,updateInfo.fw[FW_U8].upgradeFlag);
	if(updateInfo.headFlag == 0x55AA && updateInfo.fw[FW_U8].upgradeFlag == 1){
		UpdateFromAppBkp(updateInfo.fw[FW_U8].startAddrs,updateInfo.fw[FW_U8].fsize,updateInfo.fw[FW_U8].checkSum);
	}
	
    if(((*(__IO uint32_t*)(AppFlashAddr+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
    {	 
        printf("跳转到APP应用程序!\n");
		SoftDelay_ms(10);
        JumpToApplication(AppFlashAddr);//自定义的跳转函数，跳转地址为0X0000
    }
    else 
    {
        printf("非APP应用程序,无法执行!\n");   
    }	
    
    while(1)
    {
        printf("没有app程序!\n");
        SoftDelay_ms(1000);
    }
}


/*****************************************************************************
** Function name:           vApplicationTickHook
** Descriptions:               //应用层需要用到的tick级别调用,可写在该函数中 
** input parameters:       
** output parameters:       None
** Returned value:              None
** Author:                          quqian
*****************************************************************************/
void vApplicationTickHook(void)
{
 //   printf("\r\n quqian,i love you! \r\n");
}
