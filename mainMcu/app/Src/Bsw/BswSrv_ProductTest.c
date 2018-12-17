/*
 * @Author: zhoumin 
 * @Date: 2018-10-26 19:12:49 
 * @def :上位机配置
 * @Last Modified by: zhoumin
 * @Last Modified time: 2018-11-22 19:45:43
 */

#include "includes.h"
#include "BswDrv_Usart.h"
#include "BswSrv_ProductTest.h"
#include "BswDrv_Debug.h"

uint8_t gCTRecvBuffer[CT_FRAME_LEN];


/**
 *	刷卡版接收回调函数 
 **/
__weak 
void BswSrv_ProTest_RxData_Callback(uint8_t *data,uint16_t len)
{
	UNUSED(data);
	UNUSED(len);
}

int BswSrv_ProTest_SendData(uint8_t *data,uint16_t len)
{
    // PrintfData("ProTest_SendData:",data,len);
    BswDrv_UsartSend(CONFIG_UARTX_INDEX,data,len);

    return CL_OK;
}

void BswSrv_ProTest_RecvData(void)
{
    static uint8_t  step = CT_FIND_AA;
    static uint16_t pktLen;
    static uint16_t len;
    static uint8_t  sum;
    uint8_t data;
    uint8_t *pBuff = gCTRecvBuffer;

    while (CL_OK == BswDrv_UsartGetOneData(CONFIG_UARTX_INDEX,&data))
	{
        switch (step) {
            case CT_FIND_AA:
                if (0xaa == data) {
                    pktLen = 0;
                    pBuff[pktLen++] = data;
                    step = CT_FIND_55;
                }
                break;
            case CT_FIND_55:
                if (0x55 == data) {
                    step = CT_FIND_SRC;
                    pBuff[pktLen++] = data;
                    len = 0;
                }else {
                    step = CT_FIND_55;
                }
                break;

            case CT_FIND_SRC:
                pBuff[pktLen++] = data;
                if (5 <= ++len) {
                    step = CT_FIND_DEST;
                    len = 0;
                }
                break;

            case CT_FIND_DEST:
                pBuff[pktLen++] = data;
                if (5 <= ++len) {
                    step = CT_FIND_LEN;
                    len = 0;
                }
                break;

            case CT_FIND_LEN:
                pBuff[pktLen++] = data;
                if (2 <= ++len) {
                    len = (pBuff[pktLen-1]<<8) | pBuff[pktLen-2];
                    if (CT_FRAME_LEN <= len) {
                        CL_LOG("len=%d,error.\n",len);
                        step = CT_FIND_AA;
                    }else{
                        step = CT_FIND_VER;
                        sum = 0;
                    }
                }
                break;

            case CT_FIND_VER:
                pBuff[pktLen++] = data;
                sum += data;
                step = CT_FIND_SERNUM;
                break;

            case CT_FIND_SERNUM:
                pBuff[pktLen++] = data;
                sum += data;
                step = CT_FIND_CMD;
                break;

            case CT_FIND_CMD:
                pBuff[pktLen++] = data;
                sum += data;
                len = (len >= 4) ? (len - 4) : 0;
                step = len ? CT_RX_DATA : CT_FIND_CHK;
                break;

            case CT_RX_DATA:
                pBuff[pktLen++] = data;
                sum += data;
                if (0 == --len) {
                    step = CT_FIND_CHK;
                }
                break;

            case CT_FIND_CHK:
                if (sum == data) {
                    BswSrv_ProTest_RxData_Callback(pBuff,pktLen);
                }else{
                    CL_LOG("sum=%02x,pkt sum=%02x,error,drop pkt.\n",sum,data);
                }
                step = CT_FIND_AA;
                break;
		}
    }
}
