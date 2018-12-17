#include "includes.h"
#include "BswSrv_CardBoard.h"
#include "BswDrv_Usart.h"
#include "BswDrv_Debug.h"


uint8_t gCBRecvBuffer[128];

/**
 *	刷卡版接收回调函数 
 **/
__weak 
void BswSrv_CB_RxData_Callback(uint8_t *data,uint16_t len)
{
	UNUSED(data);
	UNUSED(len);
}


void BswSrv_CB_SendData(uint8_t *data,uint16_t len)
{
	// PrintfData("BswSrv_CB_SendData=",data,len);
	BswDrv_UsartSend(CARD_UART_INDEX,(void*)data,len);
}

/**
 * 接收刷卡板数据
 */ 
void BswSrv_CB_RecvData(void)
{
	uint8_t  data;
	uint8_t  *pktBuff = (void*)&gCBRecvBuffer[0];
	static uint8_t  step = CB_FIND_AA;
	static uint16_t pktLen;
	static uint16_t length;
	static uint8_t  sum;

	while (CL_OK == BswDrv_UsartGetOneData(CARD_UART_INDEX,&data)) 
	{
		switch (step) 
		{
    		case CB_FIND_AA:
    		{
    			if (data == 0xAA) {
    				pktLen=0;
    				pktBuff[pktLen] = 0xAA;
    				pktLen++;
    				step = CB_FIND_FIVE;
    			}
    		}
    		break;
    		case CB_FIND_FIVE:
    		{
    			if (data == 0x55) {
    				pktBuff[pktLen] = 0x55;
    				pktLen++;
    				step = CB_FIND_LEN;
    			} else {
    				step = CB_FIND_AA;
    			}
    		}
    		break;
    		case CB_FIND_LEN:
    		{
    			if (pktLen == 2) {
    				length = data;
    			} else if (pktLen == 3) {
    				length |= (data << 8);
    				sum = 0;
                    if (5 > length) {
                        CL_LOG("length=%d,error.\n", length);
                        step = CB_FIND_AA;
                        break;
                    }
                    step = CB_FIND_VER;
    			}
    			pktBuff[pktLen] = data;
    			pktLen++;
    		}
    		break;
    		case CB_FIND_VER:
    		{
    			if (data == 0x01) {
    				sum += data;
    				pktBuff[pktLen] = data;
    				length--;
    				pktLen++;
    				step = CB_FIND_SN;
    				//CL_LOG("CKB find version = %d.\n", data);
    			} else {
    				step = CB_FIND_AA;
    			}
    		}
    		break;
    		case CB_FIND_SN:
    		{
    			sum += data;
    			pktBuff[pktLen] = data;
    			length--;
    			pktLen++;
    			step = CB_FIND_MODULE;
    			//CL_LOG("CKB find sn = %d.\n", sn);
    		}
    		break;
    		case CB_FIND_MODULE:
    		{
    			sum += data;
    			pktBuff[pktLen] = data;
    			length--;
    			pktLen++;
    			step = CB_FIND_CMD;
    			//CL_LOG("CKB find module = %d.\n", module);
    		}
    		break;
    		case CB_FIND_CMD:
    		{
    			sum += data;
    			pktBuff[pktLen] = data;
    			if (--length) {
                    step = CB_FIND_DATA;
                }else{
                    step = CB_FIND_CHECKSUM;
                }
    			pktLen++;
    			//CL_LOG("CKB find cmd = %d.\n", cmd);
    		}
    		break;
    		case CB_FIND_DATA:
    		{
    			sum += data;
    			pktBuff[pktLen] = data;
    			pktLen++;
    			length--;
    			if (length==1) {
    				step = CB_FIND_CHECKSUM;
    			}
    		}
    		break;
    		case CB_FIND_CHECKSUM:
    		{
    			pktBuff[pktLen] = data;
    			pktLen++;
    			if (data == sum) {
    				BswSrv_CB_RxData_Callback(pktBuff,pktLen);
    			} else {
    				CL_LOG("CB checksum err.\n");
    			}
    			step = CB_FIND_AA;
    		}
    		break;
    		default:
    		{
    			step = CB_FIND_AA;
    		}
    		break;
    	}
	}
}

void BswSrv_CB_Reset()
{
	CB_POWER_OFF();
	osDelay(1000);
	CB_POWER_ON();
}

void BswSrv_CB_Init(void)
{
    //刷卡版电源
	CB_POWER_ON();
}


