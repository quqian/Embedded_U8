/*
 * @Author: zhoumin 
 * @Date: 2018-10-23 17:56:03 
 * @Last Modified by: zhoumin
 * @Last Modified time: 2018-11-08 16:29:14
 */
#include "includes.h"
#include "BswSrv_WifiBlueTask.h"
#include "BswSrv_System.h"
#include "BswDrv_Usart.h"
#include "BswDrv_Debug.h"
#include "BswDrv_Rtc.h"
#include "BswSrv_WG215.h"



//蓝牙接收处理buf
static uint8_t wBlueBuffer[CKB_MAX_PKT_LEN] = {0};

/*wifi模块底层数据接收fifo*/
extern FIFO_S_t wifiBlueFifo;
/*wifi应用层接收fifo*/
extern FIFO_S_t rWifiFifo;
/*蓝牙应用层接收fifo*/
extern FIFO_S_t rBlueFifo;

/*蓝牙和wifi初始化标志*/
uint8_t wifiInitFlag = 0;	//0--没有初始化  1--正在被初始化

///*蓝牙和wifi启动标志*/
//uint8_t wifiStartUpFlag = 0; //0-没有启动  1-已经启动

/**
 * 蓝牙接收数据回调函数定义 
 */
__weak 
void BswSrv_Blue_RxData_Callback(uint8_t *data,uint16_t len)
{
	//northing
	UNUSED(data);
	UNUSED(len);
}


__weak 
void BswSrv_Blue_ConnectState_Callback(uint8_t state)
{
	//northing
	UNUSED(state);
}

void RecvBtData(void)
{
    uint8_t  *pBuff = wBlueBuffer;
    static uint8_t  len;
    static uint8_t  pktLen;
    uint8_t  data;
    static uint8_t  step = BT_FIND_EE;
    static uint8_t  sum;
	static uint32_t time;

	if (BT_FIND_EE != step) {
        if (5000 < (uint32_t)(osGetTimes() - time)) {
            CL_LOG("too long no recv data,step=%d,error.\n",step);
            step = BT_FIND_EE;
        }
    }
	
    while (CL_OK == FIFO_S_Get(&rBlueFifo, &data)) 
    {
        // printf("%c \r\n",data);
        switch (step) {
            case BT_FIND_EE:
                if (0xee == data) {
                    pktLen = 0;
                    pBuff[pktLen++] = data;
                    step = BT_FIND_CMD;
					time = osGetTimes();
                    sum = 0xee;
                }
                break;

            case BT_FIND_CMD:
                pBuff[pktLen++] = data;
                sum += data;
                step = BT_FIND_LEN;
                break;

            case BT_FIND_LEN:
                pBuff[pktLen++] = data;
                sum += data;
                len = data;
                if (CKB_MAX_PKT_LEN < len) {
                    step = BT_FIND_EE;
                }else if (0 == len) {
                    step = BT_FIND_CHK;
                }else{
                    step = BT_RX_DATA;
                }
                break; 

            case BT_RX_DATA:
                pBuff[pktLen++] = data;
                sum += data;
                if (0 == --len) {
                    step = BT_FIND_CHK;
                }
                break;

           case BT_FIND_CHK:
                pBuff[pktLen++] = data;
                if (sum == data) 
				{
                    BswSrv_Blue_RxData_Callback(pBuff,pktLen);
                }
				else
				{
					PrintfData("RecvBtData", pBuff, pktLen);
                    CL_LOG("sum=%02x,pkt sum=%02x,error,drop pkt.\n",sum,data);
                }
                step = BT_FIND_EE;
                break;
        }
    }
}



void RecvWifiBlueToothData(void)
{
    uint8_t  data;
    static uint8_t  step = ENUM_WIFI_ZERO;
	uint8_t LenFlag = 0;
	static uint8_t CommaFlag = 0;
	static uint16_t ReceiveLen = 0;
	uint8_t ReceiveLenFlag = 0;	//
	uint32_t ReceiveWaitTime;
	uint8_t WifiReceiveLenFlag = 0;
	static uint16_t WifiReceiveLen = 0;
	uint8_t BleDisConnFlag = 0;
	uint8_t BleConnFlag = 0;
	
	ReceiveWaitTime = osGetTimes();
	while (((uint32_t)(ReceiveWaitTime + 10) >= osGetTimes()))
	{
		while (CL_OK == FIFO_S_Get(&wifiBlueFifo, &data))
		{
	        // printf("RecvWifiData = [%c],[0x%x] \r\n", data, data);
			ReceiveWaitTime = osGetTimes();
			switch(step)
			{
				case ENUM_WIFI_ZERO:
					if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
					}
				break;
				case ENUM_WIFI_IPD_I:
					if (data == 'I') 
					{
						step = ENUM_WIFI_IPD_P;
					}
					else if (data == 'W') 
					{
						step = ENUM_WIFI_WRITE_R;
					}
					else if (data == 'B') 
					{
						step = ENUM_BLEDISCONN_L;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find IPD_I.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro 1, errro 1.\n");
						#endif
					}
				break;
				case ENUM_BLEDISCONN_L:
					if (data == 'L') 
					{
						step = ENUM_BLEDISCONN_E;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find DISCONN1.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro DISCONN2, errro DISCONN2.\n");
						#endif
					}
				break;
				case ENUM_BLEDISCONN_E:
					if (data == 'E') 
					{
						step = ENUM_BLEDISCONN_D;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find DISCONN3.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro DISCONN4, errro DISCONN4.\n");
						#endif
					}
				break;
				case ENUM_BLEDISCONN_D:
					if (data == 'D') 
					{
						step = ENUM_BLEDISCONN_I;
					}
					else if (data == 'C')
					{
						step = ENUM_BLECONN_O;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find DISCONN5.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro DISCONN6, errro DISCONN6.\n");
						#endif
					}
				break;
				case ENUM_BLECONN_O:
					if (data == 'O') 
					{
						step = ENUM_BLECONN_N1;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find DISCONN7.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro DISCONN8, errro DISCONN8.\n");
						#endif
					}
				break;
				case ENUM_BLECONN_N1: case ENUM_BLECONN_N2:
					#ifdef DBG_WIFIBLUE
					// CL_LOG("qqqqqqqqqqqq[%d],[%c].\n", data, data);
					#endif

					if (data == 'N')
					{
						BleConnFlag++;
						if(1 == BleConnFlag)
						{
							step = ENUM_BLECONN_N2;
						}
						else if(2 == BleConnFlag)
						{
							step = ENUM_BLECONN_COLON;
						}
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find ENUM_BLECONN_N2.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro DISCONN8, errro ENUM_BLECONN_N2.\n");
						#endif
					}
				break;
				case ENUM_BLECONN_COLON:
					if (data == ':') 
					{
						#ifdef DBG_WIFIBLUE
						CL_LOG("蓝牙已连接! 蓝牙已连接!\n");
						#endif
						BswSrv_Blue_ConnectState_Callback(1);
						step = ENUM_WIFI_ZERO;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find CON16N.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro CON17N, errro CON17N.\n");
						#endif
					}
				break;
				case ENUM_BLEDISCONN_I:
					if (data == 'I') 
					{
						step = ENUM_BLEDISCONN_S;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find DISCONN7.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro DISCONN8, errro DISCONN8.\n");
						#endif
					}
				break;
				case ENUM_BLEDISCONN_S:
					if (data == 'S') 
					{
						step = ENUM_BLEDISCONN_C;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find DISCONN9.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro DISCON10N, errro DISCON10N.\n");
						#endif
					}
				break;
				case ENUM_BLEDISCONN_C:
					if (data == 'C') 
					{
						step = ENUM_BLEDISCONN_O;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find DISCON11N.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro DISCON12N, errro DISCON12N.\n");
						#endif
					}
				break;
				case ENUM_BLEDISCONN_O:
					if (data == 'O') 
					{
						step = ENUM_BLEDISCONN_N1;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find DISCON13N.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro DISCON14N, errro DISCON14N.\n");
						#endif
					}
				break;
				case ENUM_BLEDISCONN_N1: case ENUM_BLEDISCONN_N2:
					if (data == 'N') 
					{
						BleDisConnFlag++;
						if(1 == BleDisConnFlag)
						{
							step = ENUM_BLEDISCONN_N2;
						}
						else if(2 == BleDisConnFlag)
						{
							step = ENUM_BLEDISCONN_COLON;
						}
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find DISCON15N.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro DISCON15N, errro DISCON15N.\n");
						#endif
					}
				break;
				case ENUM_BLEDISCONN_COLON:
					if (data == ':') 
					{
						#ifdef DBG_WIFIBLUE
						CL_LOG("蓝牙已断开! 蓝牙已断开!\n");
						#endif
						BswSrv_Blue_ConnectState_Callback(0);
						step = ENUM_WIFI_ZERO;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find DISCON16N.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro DISCON17N, errro DISCON17N.\n");
						#endif
					}
				break;
				case ENUM_WIFI_IPD_P:
					if (data == 'P') 
					{
						step = ENUM_WIFI_IPD_D;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find IPD_P.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro 2, errro 2.\n");
						#endif
					}
				break;
				case ENUM_WIFI_IPD_D:
					if (data == 'D') 
					{
						step = ENUM_WIFI_IPD_COMMA;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find IPD_P.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro 4, errro 4.\n");
						#endif
					}
				break;
				case ENUM_WIFI_IPD_COMMA:
					if (data == ',') 
					{
						step = ENUM_WIFI_IPD_LENTH;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find IPD_P.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro 4, errro 4.\n");
						#endif
					}
				break;
				case ENUM_WIFI_IPD_LENTH:
					if(('0' <= data) && ('9' >= data))
					{
						WifiReceiveLenFlag++;
						switch(WifiReceiveLenFlag)
						{
							case 1:
								WifiReceiveLen = data - '0';
							break;
							case 2: case 3:
								WifiReceiveLen = (WifiReceiveLen * 10) + data - '0';
							break;
							default:
								WifiReceiveLenFlag = 0;
								WifiReceiveLen = 0;
								step = ENUM_WIFI_ZERO;
								#ifdef DBG_WIFIBLUE
								CL_LOG("errro 100, errro 100.\n");
								#endif
							break;
						}
					}
					else if(data == ':')
					{
						if((0 < WifiReceiveLenFlag) && (255 > WifiReceiveLen))
						{
							printf("WiFi接收长度为[%d]\r\n", WifiReceiveLen);
							step = ENUM_WIFI_IPD_COLON;
							WifiReceiveLenFlag = 0;
						}
						else
						{
							step = ENUM_WIFI_ZERO;	//接收数据错误
							#ifdef DBG_WIFIBLUE
							CL_LOG("errro 111, errro 111.\n");
							#endif
						}
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find WRITE_R.\n");
						#endif
					}
					else
					{
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro 12.\n");
						#endif
						step = ENUM_WIFI_ZERO;
					}
				break;
				case ENUM_WIFI_IPD_COLON:	//保存到WiFi buff
					if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find WRITE_R.\n");
						#endif
					}
					else
					{
						//printf("quqian = [%c],[%02x]\r\n", data, data);
						if (CL_OK == FIFO_S_Put(&rWifiFifo, data)) 
						{
							WifiReceiveLen--;
							if(0 == WifiReceiveLen)
							{
								step = ENUM_WIFI_ZERO;
								CL_LOG("WiFi数据接收正确 !.\n");
							}
						}
						else
						{
							#ifdef DBG_WIFIBLUE
							CL_LOG(" buff over flow error.\n");
							#endif
							osDelay(2);
						}
					}
				break;
				case ENUM_WIFI_WRITE_R:
					if (data == 'R') 
					{
						step = ENUM_WIFI_WRITE_I;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find WRITE_R.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro 3, errro 3.\n");
						#endif
					}
				break;
				case ENUM_WIFI_WRITE_I:
					if (data == 'I') 
					{
						step = ENUM_WIFI_WRITE_T;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find WRITE_R.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro 5, errro 5.\n");
						#endif
					}
				break;
				case ENUM_WIFI_WRITE_T:
					if (data == 'T') 
					{
						step = ENUM_WIFI_WRITE_E;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find WRITE_R.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro 6, errro 6.\n");
						#endif
					}
				break;
				case ENUM_WIFI_WRITE_E:
					if (data == 'E') 
					{
						step = ENUM_WIFI_WRITE_COLON;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find WRITE_R.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro 7, errro 7.\n");
						#endif
					}
				break;
				case ENUM_WIFI_WRITE_COLON:
					if (data == ':')
					{
						step = ENUM_WIFI_WRITE_COMMA;
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find WRITE_R.\n");
						#endif
					}
					else
					{
						step = ENUM_WIFI_ZERO;
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro 8, errro 8.\n");
						#endif
					}
				break;
				case ENUM_WIFI_WRITE_COMMA:
					if (data == ',')
					{
						if(CommaFlag == ',')
						{
							step = ENUM_WIFI_WRITE_LENTH;
							CommaFlag = 0;
						}
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find WRITE_R.\n");
						#endif
					}
					else							
					{
						if (10 < LenFlag++) 
						{
							LenFlag = 0;
							step = ENUM_WIFI_ZERO;
							#ifdef DBG_WIFIBLUE
							CL_LOG("errro 9, errro 9.\n");
							#endif
						}
					}
					CommaFlag = data;
				break;
				case ENUM_WIFI_WRITE_LENTH:
					//printf("hhhhhhhhhhhhh = [%c],[%02x]\r\n", data, data);
					if(('0' <= data) && ('9' >= data))
					{
						ReceiveLenFlag++;
						switch(ReceiveLenFlag)
						{
							case 1:
								ReceiveLen = data - '0';
							break;
							case 2: case 3:
								ReceiveLen = (ReceiveLen * 10) + data - '0';
							break;
							default:
								ReceiveLenFlag = 0;
								ReceiveLen = 0;
								step = ENUM_WIFI_ZERO;
								#ifdef DBG_WIFIBLUE
								CL_LOG("errro 10, errro 10.\n");
								#endif
							break;
						}
					}
					else if(data == ',')
					{
						if((0 < ReceiveLenFlag) && (255 > ReceiveLen))
						{
							printf("接收长度为[%d]\r\n", ReceiveLen);
							step = ENUM_WIFI_WRITE_DATA;
							ReceiveLenFlag = 0;
						}
						else
						{
							step = ENUM_WIFI_ZERO;	//接收数据错误
							#ifdef DBG_WIFIBLUE
							CL_LOG("errro 11, errro 11.\n");
							#endif
						}
					}
					else if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find WRITE_R.\n");
						#endif
					}
					else
					{
						#ifdef DBG_WIFIBLUE
						CL_LOG("errro 12.\n");
						#endif
						step = ENUM_WIFI_ZERO;
					}
				break;
				case ENUM_WIFI_WRITE_DATA:	//保存到蓝牙buff
					if (data == '+') 
					{
						step = ENUM_WIFI_IPD_I;
						#ifdef DBG_WIFIBLUE
						CL_LOG("can not find WRITE_R.\n");
						#endif
					}
					else
					{
						//printf("quqian = [%c],[%02x]\r\n", data, data);
						if (CL_OK == FIFO_S_Put(&rBlueFifo, data)) 
						{
							ReceiveLen--;
							if(0 == ReceiveLen)
							{
								step = ENUM_WIFI_ZERO;
								CL_LOG("数据接收正确 !.\n");
							}
						}
						else
						{
							#ifdef DBG_WIFIBLUE
							CL_LOG("blue buff over flow error.\n");
							#endif
							osDelay(2);
						}
					}
				break;
				default:
					step = ENUM_WIFI_ZERO;
				break;
			}
	    }
	}
}


void WifiBlueTask(void)
{
    uint32_t tick = 0;
	uint8_t retry = 0;

	while(1)
	{
		if(GlobalInfo.upgradeFlag != OTA_NONE)
		{
			osDelay(1000);
			continue;
		}

		if(GlobalInfo.WG215InitOK == 0)
		{
			if(BswSrv_WG215_StartUp() == CL_OK)
			{
				GlobalInfo.WG215InitOK = 1;
				CL_LOG("wifi模块重启ok..\r\n");
			}
			else
			{
				GlobalInfo.WG215InitOK = 0;
				CL_LOG("wifi模块重启失败..\r\n");
			}
		}
		
		while(GlobalInfo.WG215InitOK)
		{
			if(GlobalInfo.BlueInitOk == 0 && (osGetTimes() - tick > 5000))
			{
				if(wifiInitFlag == 0)
				{
					wifiInitFlag = 1;
					tick = osGetTimes();
					if(BswSrv_WG215_StartBlue() == CL_OK)
					{
						GlobalInfo.BlueInitOk = 1;
						wifiInitFlag = 0;
						CL_LOG("蓝牙初始化OK..\r\n");
					}
					else
					{
						GlobalInfo.BlueInitOk = 0;
						wifiInitFlag = 0;
						if(retry++ > 3)
						{
							retry = 0;
							CL_LOG("蓝牙初始化失败..\r\n");
						}
					}
				}
			}

			//处理wifi模块底层数据
			RecvWifiBlueToothData();

			//接收蓝牙数据
			RecvBtData();

			//判断蓝牙是否正常工作..how todo?
			osDelay(50);
		}
		osDelay(2000);
	}
}

