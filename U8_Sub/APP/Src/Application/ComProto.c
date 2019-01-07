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
	CL_LOG("pFrame->head.cmd = [%#x], [%d], [%d]\n", pFrame->head.cmd, pFrame->data[0], pFrame->data[1]);
	PrintfData("发送数据给产测软件", (uint8_t*)pFrame, sizeof(OUT_NET_HEAD_STR) + len + 1);
    return UsartSend(DEBUG_INDEX, (void*)pFrame, sizeof(OUT_NET_HEAD_STR) + len + 1);
}

void SendTestPktAck(uint8_t cmd, uint8_t *pkt, uint8_t len)
{
	uint8_t TestPktStrAck[128] = {0};
    OUT_PKT_STR *pFrame = (void*)TestPktStrAck;

    pFrame->head.cmd = cmd;
	pFrame->data[0] = 1;
    memcpy(&pFrame->data[1], (void*)pkt, len);
	SendDebugPkt(0, pFrame, len + 1);
}


void DebugPktProc(OUT_PKT_STR *pBuff, uint16_t len)
{
	uint16_t dataLen = 1;
    SET_RTC_STR Rtc = {0};
    int64_t timeStamp;
    rtc_parameter_struct RtcData;
    
	CL_LOG("comcom cmd = [%#x], [%d], [%d]\n", pBuff->head.cmd, pBuff->data[0], pBuff->data[1]);
	PrintfData("Debug接收数据", (uint8_t*)pBuff, len + 1);
	
    switch (pBuff->head.cmd)
	{
		case TEST_CMD_STATUS:	//请求测试
    		if(pBuff->data[0] == 0)
			{
				CL_LOG("打开上位机检测!\n");
				pBuff->data[1] = 0;
				GlobalInfo.UpperMonitorTestFlag = 0xa5;
				GlobalInfo.UpperMonitorTicks = GetTimeTicks();
			}
			else
			{
				CL_LOG("关闭上位机检测!\n");
				pBuff->data[1] = 1;
				GlobalInfo.UpperMonitorTestFlag = 0;
			}
			
			dataLen = 2;
		break;

		case TEST_CMD_VOICE://喇叭测试
			CL_LOG("DEBUG_CMD_TRUMPTE \n");
			PlayVoice(VOIC_WELCOME);
			pBuff->data[0] = 0;
			dataLen = 1;
		break;
        case DEBUG_CMD_SET_RTC:
            CL_LOG("DEBUG_CMD_SET_RTC \n");
            Rtc.SetRTC = 0;
            Rtc.SetRTC = (pBuff->data[0] << 24) + (pBuff->data[1] << 16) + (pBuff->data[2] << 8) + (pBuff->data[3]);
            SetRtcCount(Rtc.SetRTC);
          //  CL_LOG("hhhhhhhhhhhhhhhhhhhhhhhSetRtc = %d, %x", Rtc.SetRTC, Rtc.SetRTC);
          //  CL_LOG("[%X, %X, %X %X]", pBuff->data[0], pBuff->data[1], pBuff->data[2], pBuff->data[3]);
			pBuff->data[0] = 0;
			dataLen = 1;
		break;
        
        case DEBUG_CMD_RED_RTC:
            CL_LOG("DEBUG_CMD_RED_RTC \n");
            
            timeStamp = GetRtcTimeStamp();
            CL_LOG("GET RTC时间戳[%d]\n", (uint32_t)timeStamp);
			pBuff->data[0] = (timeStamp>> 24) & 0xff;
            pBuff->data[1] = (timeStamp>> 16) & 0xff;
            pBuff->data[2] = (timeStamp>> 8) & 0xff;
            pBuff->data[3] = (timeStamp) & 0xff;
      //      CL_LOG("qqqqqqqqqq[%x,%x,%x,%x] \n", pBuff->data[0], pBuff->data[1], pBuff->data[2], pBuff->data[3]);
            rtc_current_time_get(&RtcData);
      //      printf("%2x%02x%02x %02x:%02x:%02x", RtcData.rtc_year, RtcData.rtc_month, 
      //      RtcData.rtc_date, RtcData.rtc_hour, RtcData.rtc_minute, RtcData.rtc_second);
            pBuff->data[4] = BCD2HEX(RtcData.rtc_year);
            pBuff->data[5] = BCD2HEX(RtcData.rtc_month);
            pBuff->data[6] = BCD2HEX(RtcData.rtc_date);
            pBuff->data[7] = BCD2HEX(RtcData.rtc_hour);
            pBuff->data[8] = BCD2HEX(RtcData.rtc_minute);
            pBuff->data[9] = BCD2HEX(RtcData.rtc_second);
			dataLen = 10;
		break;
        
		case GET_CMD_U8_CARD:
			GlobalInfo.TestCardFlag = 0xa5;
		return;
		//break;
		case TEST_CMD_FW_VERSION:
			CL_LOG("DEBUG_CMD_GETSVER data[0]=%d \n",pBuff->data[0]);
			if(pBuff->data[0] == 0)
			{
				
			}
			else
			{
				pBuff->data[1] = (uint8_t)FW_VERSION;
				pBuff->data[2] = (uint8_t)FW_VERSION_SUB1;
                
				CL_LOG("版本号[%d.%d,%d]\n", pBuff->data[1], pBuff->data[2], pBuff->data[3]);
			}
			
			dataLen = 3;
		break;
        case DEBUG_CMD_REBOOT:
			pBuff->data[0] = 0;
            SendDebugPkt(0, pBuff, 1);
			CL_LOG("DEBUG_CMD_REBOOT重启系统！\n");
            DelayMsWithNoneOs(1000);
            NVIC_SystemReset();
		break;
		
        case DEBUG_CMD_FLASH:
            pBuff->data[0] = FlashTest();
            dataLen = 1;
		break;
		
        case GET_CMD_RS232:
			if(0xa5 == GlobalInfo.ShakeHandState)
			{
				pBuff->data[0] = 0;
			}
			else
			{
				pBuff->data[0] = 1;
			}
            
            dataLen = 1;
		break;
		default:
			CL_LOG("错误comcom cmd = [%#x], error\n", pBuff->head.cmd);
			pBuff->data[0] = 1;
		break;
//		case DEBUG_CMD_LED:
//            LedTest(pBuff);
//            return;
//        case TEST_CMD_STATUS:
//            TestStatus(pBuff);
//            return;
	}
	
    SendDebugPkt(0, pBuff, dataLen);
}


void DebugRecvProc(void)
{
    static uint8_t  step = FIND_AA;
	static uint8_t buff[256];
    static uint16_t pktLen;
    static uint16_t len;
    static uint8_t  sum;
    uint8_t data;
    uint8_t *pBuff = buff;
	uint32_t DebugTimeTicksFlag = GetTimeTicks();

	while(((DebugTimeTicksFlag + 10) >= GetTimeTicks()) && (DebugTimeTicksFlag <= GetTimeTicks()))
	{
	    while (CL_OK == FIFO_S_Get(&(UartPortHandleInfo->rxBuffCtrl), &data))
		{
			DebugTimeTicksFlag = GetTimeTicks();
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
				default:
				break;
	        }
	    }
	}
}




