#include "usart.h"
#include "includes.h"
#include "gd32f3x0_gpio.h"
#include "gd32f3x0_usart.h"
#include "gd32f3x0_rcu.h"
#include "gd32f3x0_misc.h"
#include "sim_uart.h"



#define UART_RX_BUFF_ENABLE         1



__IO UART_BUFF_STR UARTx_RxBuff[UART_COM_NUM] = {0,};




UART_INFO_STR UartPortHandleInfo[UART_COM_NUM] = {0,};

uint32_t USARTX[UART_COM_NUM] = 			{USART0, 		USART1};
rcu_periph_enum RCU_GPIOX[UART_COM_NUM] = 	{RCU_GPIOA,     RCU_GPIOA};
rcu_periph_enum RCU_USARTX[UART_COM_NUM] = {RCU_USART0,    RCU_USART1};
uint32_t RCU_USART_TX_PORT[UART_COM_NUM] = {GPIOA,         GPIOA};
uint32_t RCU_USART_RX_PORT[UART_COM_NUM] = {GPIOA,         GPIOA};

uint32_t GPIO_PIN_TX[UART_COM_NUM] = 		{GPIO_PIN_9,  	GPIO_PIN_2};
uint32_t GPIO_PIN_RX[UART_COM_NUM] = 		{GPIO_PIN_10, 	GPIO_PIN_3};

uint8_t UARTx_IRQn[UART_COM_NUM] = 		{USART0_IRQn, 	USART1_IRQn};


__IO uint8_t  gPrintBuff[256];   //最大心跳报文长度
volatile uint16_t gWrite = 0;

int fputc(int ch, FILE *f)
#if 0
{
    usart_data_transmit(USARTX[DEBUG_INDEX], (uint8_t)ch);
    while(RESET == usart_flag_get(USARTX[DEBUG_INDEX], USART_FLAG_TBE));

    return ch;
}
#else
{
    FeedWatchDog();
    if(0xa5 == GlobalInfo.UpperMonitorTestFlag)
	{
		VirtualUartByteSend(ch);
	}
	else
	{
		if (gWrite < sizeof(gPrintBuff)) 
	    {
	        gPrintBuff[gWrite++] = ch;
	        if (('\n' == ch) || ('\r' == ch) || ('\0' == ch))
	        {
	            UsartSend(DEBUG_INDEX, (void *)gPrintBuff, gWrite);
				//SimuUartSendData(&gSimUartCtrl[0], (void *)gPrintBuff, gWrite);
	            gWrite = 0;
	        }
	    }
    	else
	    {
			UsartSend(DEBUG_INDEX, (void *)gPrintBuff, sizeof(gPrintBuff));
			//SimuUartSendData(&gSimUartCtrl[0], (void *)gPrintBuff, sizeof(gPrintBuff));
	        gWrite = 0;
		}
	}
    
    return ch;
}
#endif


void PrintfData(void *pfunc, uint8_t *pdata, int len)
{
	uint32_t i;

	CL_LOG("call by %s,len=%d,pdata:",(char*)pfunc,len);
	for(i = 0; i < len; i++) 
	{
		printf("%02X", pdata[i]);
	}
	printf("\n");
}

void PrintfChar(void *pfunc, uint8_t *pdata, int len)
{
	uint32_t i;

	printf("[%s,len=%d],pdata:",(char*)pfunc,len);
	for(i = 0; i < len; i++) 
	{
		printf("%c", pdata[i]);
	}
	printf("\n");
}

int UARTx_Init(uint8_t index, ControlStatus intEnable, uint32_t Baud)
{
	UART_INFO_STR *pUartInfo = &UartPortHandleInfo[index];

//	rcu_usart_clock_config(RCU_USART0SRC_IRC8M);
    rcu_periph_clock_enable(RCU_GPIOX[index]);
    rcu_periph_clock_enable(RCU_USARTX[index]);

	gpio_af_set(RCU_USART_TX_PORT[index], GPIO_AF_1, GPIO_PIN_TX[index]);
    gpio_af_set(RCU_USART_RX_PORT[index], GPIO_AF_1, GPIO_PIN_RX[index]);

    gpio_mode_set(RCU_USART_TX_PORT[index], GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_TX[index]);
    gpio_output_options_set(GPIO_PIN_TX[index], GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_TX[index]);
	gpio_mode_set(RCU_USART_RX_PORT[index], GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_RX[index]);
    gpio_output_options_set(RCU_USART_RX_PORT[index], GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_RX[index]);
	
    usart_deinit(USARTX[index]);
    usart_baudrate_set(USARTX[index], Baud);            //波特率
//    usart_parity_config(USARTX[index], USART_PM_NONE);             //奇偶校验
//    usart_word_length_set(USARTX[index], USART_WL_8BIT);           //8位数字位
//    usart_stop_bit_set(USARTX[index], USART_STB_1BIT);             //1位停止位

    usart_receive_config(USARTX[index], USART_RECEIVE_ENABLE);     //接收使能
    usart_transmit_config(USARTX[index], USART_TRANSMIT_ENABLE);   //发送使能

	//pUartInfo->uartMutex = xSemaphoreCreateMutex();
	if(ENABLE == intEnable)
	{
		//nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
        //usart_interrupt_enable(USARTX[index], USART_INT_TBE);
		usart_interrupt_enable(USARTX[index], USART_INT_RBNE);
	    nvic_irq_enable(UARTx_IRQn[index], 2, 1);

		FIFO_S_Init(&pUartInfo->rxBuffCtrl, (void*)UARTx_RxBuff[index].RxBuff, sizeof(UARTx_RxBuff[index].RxBuff));
		FIFO_S_Flush(&pUartInfo->rxBuffCtrl);
	}

	usart_enable(USARTX[index]);
    
    return 0;
}


int UsartSend(uint8_t portIndex, char *data, uint16_t len)
{
    uint16_t i = 0;
//    UART_INFO_STR *pUart = &UartPortHandleInfo[portIndex];

	//if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (pUart->uartMutex != NULL))//系统已经运行
	//{
	//	xSemaphoreTake(pUart->uartMutex,1000);
	//}
    
    for(i = 0; i < len; i++)
    {
    	FeedWatchDog();
        usart_data_transmit(USARTX[portIndex], (uint8_t)data[i]);
        //while(RESET == usart_flag_get(pUart->uartCom, USART_FLAG_TBE));
        while(RESET == usart_flag_get(USARTX[portIndex], USART_FLAG_TBE));
    }
    DelayMsWithNoneOs(20);
	//if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (pUart->uartMutex != NULL))//系统已经运行
	//{
	//	xSemaphoreGive(pUart->uartMutex);
	//}
    
    return CL_OK;;
}

//返回值为0，读取成功，其他:读取失败
//读取一个字符后，该字符从缓存区清空
int UsartGetOneData(int portIndex, uint8_t *pData)
{
    UART_INFO_STR *pUart = &UartPortHandleInfo[portIndex];

    return FIFO_S_Get(&pUart->rxBuffCtrl, pData);
}

void UsartInit(void)
{
    UARTx_Init(DEBUG_INDEX, ENABLE, 19200);
    UARTx_Init(1, ENABLE, 115200);
}


void IRQHandler(void)
{
	uint8_t ComIndex;
	uint8_t data = 0;
	UART_INFO_STR *pUart = &UartPortHandleInfo[0];
	
	for(ComIndex = 0; ComIndex < UART_COM_NUM; ComIndex++)
	{
		if(RESET != usart_interrupt_flag_get(USARTX[ComIndex], USART_INT_FLAG_RBNE))
	    {
	    	pUart = &UartPortHandleInfo[ComIndex];
	        data = usart_data_receive(USARTX[ComIndex]);
	        FIFO_S_Put(&pUart->rxBuffCtrl, data);
	        usart_interrupt_flag_clear(USARTX[ComIndex], USART_INT_FLAG_RBNE);
			return;
	    }
	}
}


#define  IRQ_HANDLER            1
void USART0_IRQHandler(void)
{
#if IRQ_HANDLER
	IRQHandler();
#else
    uint8_t data = 0;
    UART_INFO_STR *pUart = &UartPortHandleInfo[0];
    
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
    {
        data = usart_data_receive(USART0);
        FIFO_S_Put(&pUart->rxBuffCtrl, data);
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
    }
#endif
}

void USART1_IRQHandler(void)
{
#if IRQ_HANDLER
		IRQHandler();
#else
    uint8_t data = 0;
    UART_INFO_STR *pUart = &UartPortHandleInfo[1];
	
    if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE))
    {
        data = usart_data_receive(USART1);
        FIFO_S_Put(&pUart->rxBuffCtrl, data);
        usart_interrupt_flag_clear(USART1, USART_INT_FLAG_RBNE);
    }
#endif
}







