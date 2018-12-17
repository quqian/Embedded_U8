#include "includes.h"
#include "BswDrv_Debug.h"
#include "BswDrv_Usart.h"


static __IO uint8_t  gPrintBuff[164];   //最大心跳报文长度
static volatile uint16_t gWrite = 0;

int fputc(int ch, FILE *f)
#if 0
{
    usart_data_transmit(USARTX[USARTX_INDEX], (uint8_t)ch);
    while(RESET == usart_flag_get(USARTX[USARTX_INDEX], USART_FLAG_TBE));
    
    return ch;
}
#else
{
    if (gWrite < sizeof(gPrintBuff)) 
    {
        gPrintBuff[gWrite++] = ch;
        if ('\n' == ch) 
        {
            BswDrv_UsartSend(DBG_UARTX_INDEX, (void *)gPrintBuff, gWrite);
            gWrite = 0;
        }
    }
    else
    {
		BswDrv_UsartSend(DBG_UARTX_INDEX, (void *)gPrintBuff, sizeof(gPrintBuff));
        gWrite = 0;
	}
    return ch;
}
#endif

void PrintfData(void *pfunc, uint8_t *pdata, int len)
{
	uint32_t i;

	printf("call by %s,len=%d,pdata:",(char*)pfunc,len);
	for(i = 0; i < len; i++) 
	{
		printf("%02x ", pdata[i]);
	}
	printf("\r\n");
}

