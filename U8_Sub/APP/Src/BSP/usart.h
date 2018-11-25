#ifndef __USART_H__
#define __USART_H__

#include "includes.h"
#include "FIFO.h"
#include "system.h"




#define UART_COM_NUM         		2

typedef struct
{
	uint8_t RxBuff[256];
}UART_BUFF_STR;


typedef struct
{
//	SemaphoreHandle_t uartMutex;
    FIFO_S_t rxBuffCtrl;                //接收缓存控制信息
}UART_INFO_STR;




extern UART_INFO_STR UartPortHandleInfo[UART_COM_NUM] ;

extern void PrintfChar(void *pfunc, uint8_t *pdata, int len);
extern void PrintfData(void *pfunc, uint8_t *pdata, int len);
extern int UsartSend(uint8_t portIndex,char *data,uint16_t len);
extern void UsartInit(void);
extern int UsartGetOneData(int portIndex, uint8_t *pData);

#endif





