#include "ComProto.h"
#include "system.h"
#include "usart.h"
#include "rtc.h"


int SendDebugPkt(uint8_t nodeIndex, OUT_PKT_STR *pFrame, uint16_t len)
{
    pFrame->head.aa = 0xaa;
    pFrame->head.five = 0x55;
    pFrame->head.dest[0] = nodeIndex;
    pFrame->head.len = 4 + len;
    pFrame->head.ver = 1;
    pFrame->data[len] = GetPktSum((void*)&pFrame->head.ver, len + 3);
	//FIFO_S_Flush(&UartPortHandleInfo[USARTX_INDEX].rxBuffCtrl);
    return UsartSend(DEBUG_INDEX, (void*)pFrame, sizeof(OUT_NET_HEAD_STR) + len + 1);
}


void GetFwVersion(OUT_PKT_STR *pBuff)
{
    pBuff->data[1] = (uint8_t)(FW_VERSION);
    pBuff->data[2] = (uint8_t)(FW_VERSION_SUB1);
    pBuff->data[3] = (uint8_t)FW_VERSION_SUB2;
    SendDebugPkt(0, pBuff, 4);
}


void DebugPktProc(OUT_PKT_STR *pBuff, uint16_t len)
{
    int ret = CL_OK;
	
    switch (pBuff->head.cmd) 
	{
		case TEST_CMD_FW_VERSION:
            GetFwVersion(pBuff);
            return;
#if 0
        case DEBUG_CMD_PRINTF:
            system_info.printfSwitch = pBuff->data[0];
            writeFlash = 0;
            break;
        case DEBUG_CMD_REBOOT:
            SendDebugPkt(0, pBuff, 1);
            vTaskDelay(1000);
            NVIC_SystemReset();
            break;
        case DEBUG_CMD_FLASH:
            pBuff->data[0] = FlashTest();
            SendDebugPkt(0, (void*)pBuff, 1);
            return;
		case DEBUG_CMD_LED:
            LedTest(pBuff);
            return;
        case TEST_CMD_STATUS:
            TestStatus(pBuff);
            return;
#endif
	}
    pBuff->data[0] = ret;
    SendDebugPkt(0, pBuff, 1);
}


void DebugRecvProc(void)
{
    static uint8_t  step = FIND_AA;
    static uint16_t pktLen;
    static uint16_t len;
    static uint8_t  sum;
    uint8_t data;
	static uint8_t buff[256];
    uint8_t *pBuff = buff;

    while (CL_OK == FIFO_S_Get(&(UartPortHandleInfo->rxBuffCtrl), &data))
	{
        switch (step) 
		{
            case FIND_AA:
                if (0xaa == data) 
				{
                    pktLen = 0;
                    pBuff[pktLen++] = data;
                    step = FIND_55;
                }
                break;

            case FIND_55:
                if (0x55 == data) 
				{
                    step = FIND_SRC;
                    pBuff[pktLen++] = data;
                    len = 0;
                }
				else 
				{
                    step = FIND_55;
                }
                break;

            case FIND_SRC:
                pBuff[pktLen++] = data;
                if (5 <= ++len) 
				{
                    step = FIND_DEST;
                    len = 0;
                }
                break;

            case FIND_DEST:
                pBuff[pktLen++] = data;
                if (5 <= ++len) 
				{
                    step = FIND_LEN;
                    len = 0;
                }
                break;

            case FIND_LEN:
                pBuff[pktLen++] = data;
                if (2 <= ++len) 
				{
                    len = (pBuff[pktLen-1]<<8) | pBuff[pktLen-2];
                    if (sizeof(buff) <= len) 
					{
                        CL_LOG("len=%d,error.\n",len);
                        step = FIND_AA;
                    }
					else
					{
                        step = FIND_VER;
                        sum = 0;
                    }
                }
                break;

            case FIND_VER:
                pBuff[pktLen++] = data;
                sum += data;
                step = FIND_SERNUM;
                break;

            case FIND_SERNUM:
                pBuff[pktLen++] = data;
                sum += data;
                step = FIND_CMD;
                break;

            case FIND_CMD:
                pBuff[pktLen++] = data;
                sum += data;
                len = (len >= 4) ? (len - 4) : 0;
                step = len ? RX_DATA : FIND_CHK;
                break;

            case RX_DATA:
                pBuff[pktLen++] = data;
                sum += data;
                if (0 == --len) 
				{
                    step = FIND_CHK;
                }
                break;

           case FIND_CHK:
                if (sum == data) 
				{
                    PrintfData("DebugRecvProc", pBuff, pktLen);
                    SystemInfo.printfSwitch = 0;
                    DebugPktProc((OUT_PKT_STR*)pBuff, pktLen);
                }
				else
				{
                    CL_LOG("sum=%02x,pkt sum=%02x,error,drop pkt.\n",sum,data);
                }
                step = FIND_AA;
                break;
        }
    }
}




