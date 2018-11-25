#ifndef __SIM_USART_H__
#define __SIM_USART_H__

#include "includes.h"
#include "FIFO.h"
#include "system.h"


#define SIM_UART_CNT                1



enum{
	COM_START_BIT=0,
	COM_D0_BIT,
	COM_D1_BIT,
	COM_D2_BIT,
	COM_D3_BIT,
	COM_D4_BIT,
	COM_D5_BIT,
	COM_D6_BIT,
	COM_D7_BIT,
	//COM_CHECKSUM_BIT,//9
	COM_STOP_BIT,
};


typedef struct {
    uint16_t recvIrqOk;
    uint16_t sendTimeOut;
    uint16_t byteCheckErr;
    uint16_t recvOk;
}SIM_USART_STATI_STR;


typedef struct {
    __IO uint8_t VirtualSendFlag;                  //0无操作 1接收  2发送
    __IO uint8_t recvStat;
    __IO uint8_t recvData;
    __IO uint8_t recvCnt;
    __IO uint8_t gSendStat;
    __IO uint8_t gSendData;
    uint8_t  timerIndex;
    uint8_t  baudRate;                      //0:2400 1:4800 2:9600 3:19200 4:38400
    SIM_USART_STATI_STR statis;
    
    __IO uint8_t *pSendBuf;
    __IO uint16_t allCnt;
    __IO uint16_t sendCnt;
}SIM_UART_STR;


extern int SimuUartSendData(SIM_UART_STR *pSimUart, const uint8_t *pData, uint16_t len);
extern void SimUartInit (void);


extern SIM_UART_STR gSimUartCtrl[SIM_UART_CNT];


#endif





