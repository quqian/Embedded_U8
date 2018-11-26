#include "ProtoWithMainBoard.h"
#include "system.h"
#include "usart.h"
#include "rtc.h"



START_UPGRADE_REQUEST_STR UpgradeRequestArray = {0,};

int Upgrade_FlashCheck(void)
{
	uint16_t checksum = 0;
	uint8_t  rbyte = 0;
	uint32_t i = 0;

	FeedWatchDog();
	for(i = 0; i < UpgradeRequestArray.filesize; i++)
	{
		//FeedWatchDog();
		FlashReadByte(AppUpBkpAddr + i, (void *)&rbyte, 1);
		checksum += rbyte;
        //printf("boot checksum[%#x], [%#x]\r\n", checksum, rbyte);
	}
	
	if(checksum != UpgradeRequestArray.checkSum)
	{
        printf("校验出错!!![%#x], [%#x]\r\n", checksum, UpgradeRequestArray.checkSum);
		return 1;
	}
	
	return 0;
}

/*****************************************************************************
** Function name:       Upgrade_Write
** Descriptions:        
** input parameters:    pMsg: 内层协议数据头指针
** output parameters:   None
** Returned value:	  None
** Author:              quqian
*****************************************************************************/
uint8_t Upgrade_Write(uint8_t *pData,uint16_t len)
{
    SYS_UPDATE_INFO_STR info; 
	SYS_UPDATE_INFO_STR UpgradeInfo; 
    uint32_t i = 0;
	
    //PrintfData("升级接收数据",pData,len);
    
	if(1 == FlashWrite(((uint32_t)AppUpBkpAddr + GlobalInfo.UpgradeFw.ReceiveSize), pData, len))
	{
		printf("11111111111111111, error!");
		return 1;
	}
    //CL_LOG("upgrade current package=%d.\n",fw_info.current_package);
    CL_LOG("total %d,get %d, [%d%%].\n", UpgradeRequestArray.filesize, (GlobalInfo.UpgradeFw.ReceiveSize + len), (GlobalInfo.UpgradeFw.ReceiveSize + len) * 100 / UpgradeRequestArray.filesize);
    if((GlobalInfo.UpgradeFw.CurrentPackage == UpgradeRequestArray.package) && (0 < UpgradeRequestArray.package) && 
		(UpgradeRequestArray.filesize == (GlobalInfo.UpgradeFw.ReceiveSize + len)))
    {
        CL_LOG("最后一个包已经写完, 开始校验.\n");
		printf("CurrentPackage = [%d], [%d]\n", GlobalInfo.UpgradeFw.CurrentPackage, UpgradeRequestArray.package);
		printf("ReceiveSize = [%d], [%d]\n", GlobalInfo.UpgradeFw.ReceiveSize + len, UpgradeRequestArray.filesize);
		CL_LOG("GlobalInfo.UpgradeFw.AddrOffset = [%#x].\n", GlobalInfo.UpgradeFw.ReceiveSize);
        //数据包接受完成 校验数据和
        if(Upgrade_FlashCheck() != 0)
		{
            CL_LOG("upgrade checksum error.\n");
            return 1;
        }
        //写入消息头
        
		for(i = 0; i < 3; i++)
		{
			memset(&info, 0, sizeof(info));
			memset(&UpgradeInfo, 0, sizeof(UpgradeInfo));
			info.checkSum = UpgradeRequestArray.checkSum;
	        info.size = UpgradeRequestArray.filesize;
	        info.updateFlag = 0xAA55BCDE;
		    {
		    	fmc_unlock();
				fmc_page_erase(UPGRADE_INFO);//擦除这个扇区
				fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
				fmc_lock();
		    }
			FlashWrite((uint32_t)(UPGRADE_INFO), (void*)&info, sizeof(info));
			
			FlashReadByte((uint32_t)UPGRADE_INFO, (void *)&UpgradeInfo, sizeof(UpgradeInfo));
			DelayMsWithNoneOs(20);
			FlashReadByte((uint32_t)UPGRADE_INFO, (void *)&info, sizeof(info));
			if((info.checkSum == UpgradeRequestArray.checkSum) && (info.size == UpgradeRequestArray.filesize) && (info.updateFlag == 0xAA55BCDE) &&
				(info.checkSum == UpgradeInfo.checkSum) && (info.size == UpgradeInfo.size) && (info.updateFlag == UpgradeInfo.updateFlag))
			{
				CL_LOG("upgrade success.\n");
				PlayVoice(VOIC_SUCCESS);
				//CL_LOG("upgrade success. size=%d checksum=%X\n", fw_info.size,fw_info.checkSum);
				return 2;
			}
			else
			{
	            CL_LOG("upgrade error, [%#x], [%#x], [%#x], error.\n", info.checkSum, info.size, info.updateFlag);
	            CL_LOG("UpgradeRequestArray upgrade error, [%#x], [%#x], error.\n", UpgradeRequestArray.checkSum, UpgradeRequestArray.filesize);
				if(3 == i)
				{
					return 1;
				}
			}
		}
    }
    else
	{
        GlobalInfo.UpgradeFw.CurrentPackage++;
		GlobalInfo.UpgradeFw.ReceiveSize += len;
    }
    
    return 0;
}

/*****************************************************************************
** Function name:       COM_Upgrade_Write
** Descriptions:        
** input parameters:    pMsg: 内层协议数据头指针
** output parameters:   None
** Returned value:	  	None
** Author:              quqian
*****************************************************************************/
uint8_t COM_Upgrade_Write(uint8_t *data, uint16_t datalen)
{
	uint8_t index = data[0];
	uint8_t *pData = &data[1];
	uint16_t len = datalen - 10;
//	static uint32_t checkSum = 0;
//	uint32_t i = 0;
//	
//	for(i = 0; i < len; i++)
//	{
//		checkSum += pData[i];
//	}
//	CL_LOG("checkSum=[%#X], [%#X], [%d]\n", checkSum, len, index);
	//CL_LOG("index %d. \n", index);
	//CL_LOG("upgradeInfo.current_package = %d, len = %d. \n", fw_info.current_package, len);
//	CL_LOG("aaaaaaa[%#X], [%d]\n", len, index);
//	PrintfData("qqqqqqqq", pData, len);
	if((len > 64))
	{
		CL_LOG("len [%d], [%d],error!\n", len, index);
		return 1;
	}
	if((GlobalInfo.UpgradeFw.CurrentPackage != 1))
	{
		if((GlobalInfo.UpgradeFw.lastIndex == index))
		{
			CL_LOG("current pacakge is repeat transmate.[%d], [%d]!\n", len, index);
			return 0;
		}
	}
	GlobalInfo.UpgradeFw.lastIndex = index;
	
	return Upgrade_Write(pData, len);
}

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
		ShakeHand->cardState = 1;
	}
	else
	{
		ShakeHand->cardState = 0;
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
		printf("收到主板握手应答!\r\n");
	}
    else
	{
		printf("主板握手应答, 错误!\r\n");
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
		HeartBeat->cardState = 1;
	}
	else
	{
		HeartBeat->cardState = 0;
	}
	

	App_CB_SendData(pBuff, sizeof(HEARTBAT_STR), ENUM_MODUL_BASE, ENUM_HEART_BEAT);
	
    return CL_OK;
}

int BasicInfoHeartBeatAck(CB_STR_t *pBuff, uint16_t len)
{
	HEARTBAT_ACK_STR* HeartBeatAck = (HEARTBAT_ACK_STR*)(pBuff->data);
	
	if(0 == HeartBeatAck->result)
	{
		printf("收到主板心跳应答!\r\n");
	}
	else
	{
		printf("主板心跳应答, 错误!\r\n");
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
		printf("收到主板读卡应答!\r\n");
		//Sc8042bSpeech(VOIC_CARD);
		//DelayMsWithNoneOs(100);
	}
	else
	{
		printf("主板读卡应答, 错误!\r\n");
		return CL_FAIL;
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


int UpgradeRequestHandler(CB_STR_t *pBuff, uint16_t len)
{
	uint32_t i = 0;
	pSTART_UPGRADE_REQUEST_STR pUpgradeRequest = (START_UPGRADE_REQUEST_STR*)(pBuff->data);;
	
	UpgradeRequestArray.filesize = pUpgradeRequest->filesize;
	UpgradeRequestArray.package = pUpgradeRequest->package;
	UpgradeRequestArray.checkSum = pUpgradeRequest->checkSum;
	UpgradeRequestArray.fw_verson = pUpgradeRequest->fw_verson;
	CL_LOG("filesize[%#x], package[%#x], checkSum[%#x], fw_verson[%#x].\n", UpgradeRequestArray.filesize, UpgradeRequestArray.package, UpgradeRequestArray.checkSum, UpgradeRequestArray.fw_verson);
	
	GlobalInfo.UpgradeFw.CurrentPackage = 1;
	GlobalInfo.UpgradeFw.lastIndex = 0;
	GlobalInfo.UpgradeFw.ReceiveSize = 0;
	if((0 == UpgradeRequestArray.filesize) || (0 == UpgradeRequestArray.package)/* || 
		(UpgradeRequestArray.fw_verson == FW_VERSION)*/)
	{
        CL_LOG("filesize or package or fw_verson, error!!!");
		return CL_FAIL;
	}
#if 1
	fmc_unlock();
	for (i = 0; i < (uint32_t)(APP_FW_SIZE / FLASH_PAGE_SIZE); i++)     //擦除APP程序区 
    {
		fmc_page_erase(AppUpBkpAddr + i * FLASH_PAGE_SIZE);//擦除这个扇区
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }
	fmc_lock();
#endif

	return CL_OK;
}

int UpgradeRequestAck(uint8_t result)
{
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	START_UPGRADE_REQUEST_ACK_STR* UpgradeAck = (START_UPGRADE_REQUEST_ACK_STR*)(pBuff->data);
	
	UpgradeAck->result = result;
	App_CB_SendData(pBuff, sizeof(START_UPGRADE_REQUEST_ACK_STR), ENUM_MODUL_UPGRADE, ENUM_UPGRADE_REQUEST);
	PrintfData("UpgradeRequestAck", (uint8_t*)pBuff, sizeof(START_UPGRADE_REQUEST_ACK_STR) + sizeof(CB_HEAD_STR) + 2);
    
    return CL_OK;
}

int SendUpgradePackageAck(uint8_t result, uint32_t index)
{
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	DOWN_LOAD_FW_STR_ACK* SendUpgradeAck = (DOWN_LOAD_FW_STR_ACK*)(pBuff->data);
	
	SendUpgradeAck->result = result;
	SendUpgradeAck->index = index;
	App_CB_SendData(pBuff, sizeof(DOWN_LOAD_FW_STR_ACK), ENUM_MODUL_UPGRADE, ENUM_SEND_UPGRADE_PKT);
//	PrintfData("SendUpgradePackageAck", (uint8_t*)pBuff, sizeof(DOWN_LOAD_FW_STR_ACK) + sizeof(CB_HEAD_STR) + 2);
    
    return CL_OK;
}

int SendUpgradePackage(CB_STR_t *pBuff, uint16_t len)
{
	int ret = 0;
	uint8_t result = 0;
	pDOWN_LOAD_FW_STR DownLoadData = (pDOWN_LOAD_FW_STR)(pBuff->data);
	
//	CL_LOG("aaaaaaaaaaaa[%d]!\n", DownLoadData->index);
	
	//GetPktSum(uint8_t *pData, uint16_t len);
	if(GlobalInfo.UpgradeFlag != 0xa5)
	{
		result = 1;
    	goto SendUpgradeAck;
	}
	ret = COM_Upgrade_Write(&DownLoadData->index, len);
	if(ret == 2)
    {
    	SendUpgradePackageAck(0, DownLoadData->index);
		DelayMsWithNoneOs(1000);
		PlayVoice(VOIC_DEVICE_REBOOT);
		CL_LOG("DEBUG_CMD_REBOOT重启系统！\n");
		GlobalInfo.UpgradeFlag = 0;
		DelayMsWithNoneOs(1000);
		NVIC_SystemReset();
	}
	if(ret == 0)
    {
		result = 0;
	}
    else
	{
		result = 1;
		CL_LOG("升级错误!!!\n");
	}
	
	//DownLoadData->index = 
SendUpgradeAck: SendUpgradePackageAck(result, DownLoadData->index);
	
    return CL_OK;
}


void MainBoardUpgrade(CB_STR_t *pBuff, uint16_t len)
{
    int ret = 0;
	
    switch (pBuff->head.cmd) 
	{
		case ENUM_UPGRADE_REQUEST:
			PrintfData("ENUM_UPGRADE_REQUEST", (void*)pBuff, len);
			if(CL_OK == UpgradeRequestHandler(pBuff, len))
			{
				ret = 0;
				PlayVoice(VOIC_START_UPGRADE); 
				GlobalInfo.UpgradeFlag = 0xa5;
    			//DelayMsWithNoneOs(900);
			}
			else
			{
				CL_LOG("不可以升级!!!\n");
				ret = 1;
			}
			UpgradeRequestAck(ret);
            return;
		case ENUM_SEND_UPGRADE_PKT:
			//PrintfData("ENUM_SEND_UPGRADE_PKT", (void*)pBuff, len);
            SendUpgradePackage(pBuff, len);

            return;
	}
}

void MainBoardPktProc(CB_STR_t *pBuff, uint16_t len)
{
    //PrintfData("MainBoardPktProc", (void*)pBuff, len);
    FeedWatchDog();
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
	uint8_t gCBRecvBuffer[256] = {0,};
	uint8_t *pktBuff = (void*)&gCBRecvBuffer[0];
	static uint8_t  step = CB_FIND_AA;
	static uint16_t pktLen;
	static uint16_t length;
	static uint8_t  sum;
	uint32_t TimeTicksFlag = GetTimeTicks();

	while(((TimeTicksFlag + 10) >= GetTimeTicks()) && (TimeTicksFlag <= GetTimeTicks()))
	{
		FeedWatchDog();
		while (CL_OK == FIFO_S_Get(&(UartPortHandleInfo[1].rxBuffCtrl), &data)) 
		{
			FeedWatchDog();
			TimeTicksFlag = GetTimeTicks();
			//printf("mm:%x", data);
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
}





