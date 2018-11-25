#include "ProtoWithMainBoard.h"
#include "system.h"
#include "usart.h"
#include "rtc.h"



void App_CB_SendData(CB_STR_t *ptk,uint16_t len, uint8_t  module, uint8_t  cmd)
{
    ptk->head.aa = 0xAA;
	ptk->head.five = 0x55;
    ptk->head.sn = GlobalInfo.packageSn++;
    ptk->head.len = 5 + len;
    ptk->head.ver = 0x01;
	ptk->head.module = module;
	ptk->head.cmd = cmd;
	
    ptk->data[len] = GetPktSum((uint8_t*)&ptk->head.ver, len + 4);

	UsartSend(RS232_INDEX, (void*)ptk, sizeof(CB_HEAD_STR) + len + 1);
}

int BasicInfoShakeHand(void)
{
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	SHAKE_HAND_STR* ShakeHand = (SHAKE_HAND_STR*)pBuff->data;

	ShakeHand->fw_verson = FW_VERSION;
	if(0xa5 == GlobalInfo.card_state)
	{
		ShakeHand->cardState = 0;
	}
	else
	{
		ShakeHand->cardState = 1;
	}

	App_CB_SendData(pBuff, sizeof(SHAKE_HAND_STR), ENUM_MODUL_BASE, ENUM_SHAKE_HAND);
	PrintfData("BasicInfoShakeHand", (uint8_t*)pBuff, sizeof(SHAKE_HAND_STR) + sizeof(CB_HEAD_STR) + 2);
	
    return CL_OK;
}

int BasicInfoShakeHandAck(CB_STR_t *pBuff, uint16_t len)
{
	SHAKE_HAND_ACK_STR* ShakeHandAck = (SHAKE_HAND_ACK_STR*)(pBuff->data);
			
	if(0 == ShakeHandAck->result)
	{
		GlobalInfo.ShakeHandState = 0xa5;
		printf("收到主板握手应答! 收到主板握手应答! 收到主板握手应答!\r\n");
	}
    else
	{
		printf("主板握手应答, 错误! 主板握手应答, 错误! 主板握手应答, 错误!\r\n");
	}
	
    return CL_OK;
}

int BasicInfoHeartBeat(void)
{
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	HEARTBAT_STR* HeartBeat = (HEARTBAT_STR*)pBuff->data;

	if(0xa5 == GlobalInfo.card_state)
	{
		HeartBeat->cardState = 0;
	}
	else
	{
		HeartBeat->cardState = 1;
	}
	

	App_CB_SendData(pBuff, sizeof(HEARTBAT_STR), ENUM_MODUL_BASE, ENUM_HEART_BEAT);
	
    return CL_OK;
}

int BasicInfoHeartBeatAck(CB_STR_t *pBuff, uint16_t len)
{
	HEARTBAT_ACK_STR* HeartBeatAck = (HEARTBAT_ACK_STR*)(pBuff->data);
			
	if(0 == HeartBeatAck->result)
	{
		printf("收到主板心跳应答! 收到主板心跳应答! 收到主板心跳应答!\r\n");
	}
	else
	{
		printf("主板心跳应答, 错误! 主板心跳应答, 错误! 主板心跳应答, 错误!\r\n");
	}
	
    return CL_OK;
}


void MainBoardBasicInfo(CB_STR_t *pBuff, uint16_t len)
{
    switch (pBuff->head.cmd) 
	{
		case ENUM_SHAKE_HAND:
            //BasicInfoShakeHand(pBuff, len);
			BasicInfoShakeHandAck(pBuff, len);
            return;
		case ENUM_HEART_BEAT:
            BasicInfoHeartBeatAck(pBuff, len);
            return;
	}
}

int SwipeCardReadCard(void)
{
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	READ_CARDINFO_STR* ReadCard = (READ_CARDINFO_STR*)pBuff->data;

	ReadCard->cardType = 0;		//查看充电网M1操作规范
	memcpy(ReadCard->cardId, GlobalInfo.current_usr_card_id, 16);

	App_CB_SendData(pBuff, sizeof(READ_CARDINFO_STR), ENUM_MODUL_CARD, ENUM_READ_CARD);
	printf("卡号上报!\r\n");

    return CL_OK;
}

int SwipeCardReadCardAck(CB_STR_t *pBuff, uint16_t len)
{
	READ_CARDINFO_ACK_STR* ReadCardAck = (READ_CARDINFO_ACK_STR*)(pBuff->data);
			
	if(0 == ReadCardAck->result)
	{
		printf("收到主板读卡应答! 收到主板读卡应答! 收到主板读卡应答!\r\n");
		GlobalInfo.ReadCardFlag = 0;
		Sc8042bSpeech(VOIC_CARD);
	}
	else
	{
		printf("主板读卡应答, 错误! 主板读卡应答, 错误! 主板读卡应答, 错误!\r\n");
	}
    
    return CL_OK;
}


int SwipeCardWriteCard(CB_STR_t *pBuff, uint16_t len)
{
#if 0
	uint8_t keyA[6];
	uint8_t sector ,block, sectorBlock;
	uint8_t PICC_ATQA1[2],PICC_SAK1[3],PICC_UID1[4];

	memcpy(keyA,st.msg,6);

	sector = st.msg[6];
	block = st.msg[7];

	sectorBlock = sector*4 + block;
	//Debug_Log("写卡keyA :");
	//Debug_Hex(&keyA[0], 6);
	//Debug_Log("\r\n");

	///Debug_Log("写shuju到卡数据 :");
	//Debug_Hex(&st.msg[0],16+8);
	//Debug_Log("\r\n");

	Debug_Log("写卡寻卡\r\n");
	TypeA_Halt();
	//寻卡
	if (TypeA_CardActivate(PICC_ATQA1,PICC_UID1,PICC_SAK1) != OK)
	{
	    Debug_Log("WRITE failed.\r\n");
	    TypeA_Halt();
	    WriteCardBlockSuccessUpLoad(WRITE_ERROR);
	    return;
	}
	//Debug_Log("写卡寻卡通过\r\n");
	//Debug_Log("写卡密码认证 \r\n");
	//密码认证
	if(Mifare_Auth(0, sector, keyA, PICC_UID1) != OK)
	{
	    Debug_Log("WRITE failed.\r\n");
	    TypeA_Halt();
	    WriteCardBlockSuccessUpLoad(WRITE_ERROR);
	    return;
	}
	//Debug_Log("写卡密码认证通过\r\n");

	// Debug_Log("写卡数据到块");
	//Debug_Hex(&sectorBlock,1);
	//Debug_Log("\r\n");

	//Debug_Log("写到卡数据 :");
	//Debug_Hex(((WRITE_CARD_REQ_STR*)st.msg)->blockData,16);
	//Debug_Log("\r\n");
	switch(sectorBlock)
	{
	    case 8:
	        //Debug_Log("写块8校验\r\n");
	        if(((WRITE_CARD_REQ_STR*)st.msg)->blockData[15] == GetPktSum(&((WRITE_CARD_REQ_STR*)st.msg)->blockData[0], 15))
	        {
	            //Debug_Log("写卡块8校验通过\r\n");
	            //Debug_Log("写卡块8\r\n");
	            if(OK != Mifare_Blockwrite(8, &((WRITE_CARD_REQ_STR*)st.msg)->blockData[0]))
	            {
	                Debug_Log("写卡块8失败\r\n");
	                WriteCardBlockSuccessUpLoad(WRITE_ERROR);
	            }
	            else{
	                //Debug_Log("写卡块8成功\r\n");
	                WriteCardBlockSuccessUpLoad(WRITE_SUCCESS);
	            }
	        }
	    break;
	    default:
	        if(OK != Mifare_Blockwrite(sectorBlock, &((WRITE_CARD_REQ_STR*)st.msg)->blockData[0]))
	        {
	            Debug_Log("写卡块x失败\r\n");
	            WriteCardBlockSuccessUpLoad(WRITE_ERROR);
	        }
	        else{
	            //Debug_Log("写卡块x成功\r\n");
	            WriteCardBlockSuccessUpLoad(WRITE_SUCCESS);
	        }
	    break;
	}
	TypeA_Halt();
#endif
    
    return CL_OK;
}



int SwipeCardWriteCardAck(CB_STR_t *pBuff, uint16_t len)
{
	WRITE_CARDINFO_ACK_STR* WriteCardAck = (WRITE_CARDINFO_ACK_STR*)(pBuff->data);
			
	if(0 == WriteCardAck->result)
	{
		
	}
    
    return CL_OK;
}


void MainBoardSwipeCard(CB_STR_t *pBuff, uint16_t len)
{
    int ret = CL_OK;
	
    switch (pBuff->head.cmd) 
	{
		case ENUM_READ_CARD:
            SwipeCardReadCardAck(pBuff, len);
            return;
		case ENUM_WRITE_CARD:
            SwipeCardWriteCardAck(pBuff, len);
            return;
	}
    pBuff->data[0] = ret;
	App_CB_SendData(pBuff, 1, ENUM_MODUL_CARD, ENUM_READ_CARD);
}

int UpgradeRequest(CB_STR_t *pBuff, uint16_t len)
{
	
    return CL_OK;
}

int UpgradeRequestAck(CB_STR_t *pBuff, uint16_t len)
{
	START_UPGRADE_REQUEST_ACK_STR* UpgradeAck = (START_UPGRADE_REQUEST_ACK_STR*)(pBuff->data);
			
	if(0 == UpgradeAck->result)
	{
	
	}
    
    return CL_OK;
}


int SendUpgradePackageAck(CB_STR_t *pBuff, uint16_t len)
{
	
    return CL_OK;
}

int SendUpgradePackage(CB_STR_t *pBuff, uint16_t len)
{
	DOWN_LOAD_FW_STR_ACK* SendUpgradeAck = (DOWN_LOAD_FW_STR_ACK*)(pBuff->data);
			
	if(0 == SendUpgradeAck->result)
	{
	
	}
    
    return CL_OK;
}


void MainBoardUpgrade(CB_STR_t *pBuff, uint16_t len)
{
    int ret = CL_OK;
	
    switch (pBuff->head.cmd) 
	{
		case ENUM_UPGRADE_REQUEST:
            UpgradeRequestAck(pBuff, len);
            return;
		case ENUM_SEND_UPGRADE_PKT:
            SendUpgradePackageAck(pBuff, len);
            return;
	}
    pBuff->data[0] = ret;
    App_CB_SendData(pBuff, 1, ENUM_MODUL_UPGRADE, ENUM_UPGRADE_REQUEST);//
}

void MainBoardPktProc(CB_STR_t *pBuff, uint16_t len)
{
    switch (pBuff->head.module)
	{
		case ENUM_MODUL_BASE:
			MainBoardBasicInfo(pBuff, len);
		break;
		case ENUM_MODUL_CARD:
			MainBoardSwipeCard(pBuff, len);
		break;
		case ENUM_MODUL_UPGRADE:
			MainBoardUpgrade(pBuff, len);
		break;
	}
}

void ComRecvMainBoardData(void)
{
	uint8_t data;
	static uint8_t gCBRecvBuffer[256] = {0,};
	uint8_t *pktBuff = (void*)&gCBRecvBuffer[0];
	static uint8_t  step = CB_FIND_AA;
	static uint16_t pktLen;
	static uint16_t length;
	static uint8_t  sum;

	while (CL_OK == FIFO_S_Get(&(UartPortHandleInfo[1].rxBuffCtrl), &data)) 
	{
		printf("mm:%x", data);
		switch (step) 
		{
    		case CB_FIND_AA:
    		{
    			if (data == 0xAA) 
				{
    				pktLen=0;
    				pktBuff[pktLen] = 0xAA;
    				pktLen++;
    				step = CB_FIND_FIVE;
    			}
    		}
    		break;
    		case CB_FIND_FIVE:
    		{
    			if (data == 0x55) 
				{
    				pktBuff[pktLen] = 0x55;
    				pktLen++;
    				step = CB_FIND_LEN;
    			} 
				else 
				{
    				step = CB_FIND_AA;
    			}
    		}
    		break;
    		case CB_FIND_LEN:
    		{
    			if (pktLen == 2) 
				{
    				length = data;
    			} 
				else if (pktLen == 3) 
				{
    				length |= (data << 8);
    				sum = 0;
                    if (5 > length) 
					{
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
    			if (data == 0x01) 
				{
    				sum += data;
    				pktBuff[pktLen] = data;
    				length--;
    				pktLen++;
    				step = CB_FIND_SN;
    				//CL_LOG("CKB find version = %d.\n", data);
    			} 
				else 
				{
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
    			if (--length) 
				{
                    step = CB_FIND_DATA;
                }
				else
				{
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
    			if (length==1) 
				{
    				step = CB_FIND_CHECKSUM;
    			}
    		}
    		break;
    		case CB_FIND_CHECKSUM:
    		{
    			pktBuff[pktLen] = data;
    			pktLen++;
    			if (data == sum) 
				{
    				MainBoardPktProc((CB_STR_t*)pktBuff, pktLen);
    			} 
				else 
				{
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





