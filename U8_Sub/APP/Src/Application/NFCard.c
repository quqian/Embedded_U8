#include "NFCard.h"
#include "system.h"
#include "usart.h"
#include "fm175xx.h"
#include "m1_card.h"
#include "aes.h"
#include "rtc.h"
#include "ComProto.h"



#define USER_NFC_CARD     			0


//主区域
#define SECTOR_MAIN					12
#define BLOCK_MAIN					49

//备用区域
#define	SECTOR_BAK					2
#define BLOCK_BAK					9

static unsigned char SecretCardKEY_A[6] = {'6', 'f', '7', 'd', '2', 'k'};
static unsigned char SeedKeyA[16]="chargerlink1234";


/**
 *mode:0-退出睡眠模式
 *     1-进入睡眠模式
 */ 
int BswDrv_FM175XX_SetPowerDown(uint8_t mode)
{
    uint8_t t = 0;
    if((Read_Reg(CommandReg) & 0x10) == mode )
    {
        return CL_OK;
    }
    if(mode == 0)
    {
        Clear_BitMask(CommandReg,0x10);
        while(1)
        {
            DelayMsWithNoneOs(10);
            if((Read_Reg(CommandReg) & 0x10) == 0)
            {
                return CL_OK;
            }
            if(t++ >= 80)
            {
                CL_LOG("SetPowerDown error.\r\n");
                return CL_FAIL;
            }
        }
    }
    else if(mode == 1)
    {
        Set_BitMask(CommandReg,0x10);
    }

    return CL_OK;
}

//读卡处理
void Check_M1_Card()
{
#if 1
	uint8_t PICC_ATQA[2],PICC_SAK[3],PICC_UID[4];
	uint8_t pCardId[8];
	
	if (TypeA_CardActivate(PICC_ATQA,PICC_UID,PICC_SAK) == OK)
	{
//		PrintfData("PICC_UID:",PICC_UID,4);
//        PrintfData("PICC_ATQA1:",PICC_ATQA,2);
		//密钥卡
		if(Mifare_Auth(0,2,SecretCardKEY_A,PICC_UID) == OK)
        {
			CL_LOG("this is SecretCard.\n");
			//Sc8042bSpeech(VOIC_CARD);
		}
        else
		{//非密钥卡
			uint8_t ucardkeyA[16]={0};
            uint8_t card[16];
			AES_KEY aes;
			uint8_t result = 0;
			
			memset(&aes,0,sizeof(AES_KEY));
			AES_set_encrypt_key(SeedKeyA, 128, &aes);
            
			memset(card,0,16);
			memcpy(card,PICC_UID,4);
			AES_encrypt(card, ucardkeyA, &aes);
            
			if((TypeA_CardActivate(PICC_ATQA, PICC_UID, PICC_SAK)==OK))
            {
				uint8_t block_data[16];
				memset((void*)block_data,0,16);
			//    PrintfData("PICC_ATQA2:",PICC_ATQA,2);

				//主扇区密码认证
				if(Mifare_Auth(0,SECTOR_MAIN,ucardkeyA,PICC_UID) == OK)
				{
					if(Mifare_Blockread(BLOCK_MAIN,block_data) == OK) 
					{
						result = 0xae;
					}
				}
				else
				{
					if((TypeA_CardActivate(PICC_ATQA,PICC_UID,PICC_SAK)==OK))
					{
						if(Mifare_Auth(0,SECTOR_BAK,ucardkeyA,PICC_UID) == OK)//备份区域密码认证
						{
							if(Mifare_Blockread(BLOCK_BAK,block_data) == OK)
							{
								result = 0xae;
							}
						}
						else
						{
							CL_LOG("卡片密码认证失败...\r\n");
							Sc8042bSpeech(VOIC_CARD_INVALID);
						}
					}
				}
			   	if(0xae == result)
				{
					#if 0
					sprintf((char*)GlobalInfo.current_usr_card_id, "%02x%02x%02x%02x%02x",
							block_data[4],block_data[5],block_data[6],block_data[7],block_data[8]);
                    CL_LOG("block_data[4] = %02x ,block_data[5] = %02x ,block_data[6] = %02x ,block_data[7] = %02x ,block_data[8] = %02x .\n",
                        block_data[4],block_data[5],block_data[6],block_data[7],block_data[8]);
					
					StringToBCD(pCardId, (const char *)&GlobalInfo.current_usr_card_id[0]);
				    PrintfData("卡号", &pCardId[0], 5);
					#else
					//if(0x0f < block_data[4])
					{
						sprintf((char*)GlobalInfo.current_usr_card_id, "%02x%02x%02x%02x%02x",
							block_data[4],block_data[5],block_data[6],block_data[7],block_data[8]);
                    	CL_LOG("block_data[4] = %02x ,block_data[5] = %02x ,block_data[6] = %02x ,block_data[7] = %02x ,block_data[8] = %02x .\n",
                        	block_data[4],block_data[5],block_data[6],block_data[7],block_data[8]);
					}
                    StringToBCD(pCardId, (const char *)&GlobalInfo.current_usr_card_id[0]);
				    //PrintfData("卡号", &pCardId[0], 5);
                    if(0 == (pCardId[4] & 0x0f))
                    {
                        CL_LOG("[卡号: %02x%02x%02x%02x%01x ]\n", pCardId[0], pCardId[1], pCardId[2], pCardId[3], pCardId[4]>>4);
                    }
                    else
                    {
                        CL_LOG("[卡号: %02x%02x%02x%02x%02x ]\n", pCardId[0], pCardId[1], pCardId[2], pCardId[3], pCardId[4]);
                    }
                    
					//PrintfData("卡号", &GlobalInfo.current_usr_card_id[0], 16);
					//memcpy((char*)GlobalInfo.current_usr_card_id, &block_data[4], 5);
					#endif
				
					GlobalInfo.ReadCardFlag = 0xa5;
				}
			}
		}
		
		TypeA_Halt();
	}
#endif
}



void ReadCardHandle(void)
{
    //读卡处理
    Check_M1_Card();

	if((GlobalInfo.ReadCardFlag == 0xa5))
	{
        FeedWatchDog();
        GlobalInfo.ReadCardFlag = 0;
		if(GlobalInfo.TestCardFlag == 0xa5) 
		{
			GlobalInfo.TestCardFlag = 0;
			Sc8042bSpeech(VOIC_READING_CARD);
			SendTestPktAck(GET_CMD_U8_CARD, (void*)GlobalInfo.current_usr_card_id, sizeof(GlobalInfo.current_usr_card_id));
			return;
	    }
		else
		{
			if(((GlobalInfo.ReportTicks + 3000) <= GetTimeTicks()) || (GlobalInfo.ReportTicks > GetTimeTicks()))
	    	{
	        	GlobalInfo.ReportTicks = GetTimeTicks();
				Sc8042bSpeech(VOIC_READING_CARD);
	            SwipeCardReadCard();
	        }
		}
	}
}

void NFCardTask(void)
{
	FeedWatchDog();
    ReadCardHandle();
}


