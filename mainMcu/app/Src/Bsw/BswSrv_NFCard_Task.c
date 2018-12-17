/*
 * @Author: zhoumin 
 * @Date: 2018-10-18 15:10:28 
 * @def: 读卡器和按键任务
 * @Last Modified by: zhoumin
 * @Last Modified time: 2018-10-19 17:32:35
 */

#include "BswSrv_NFCard_Task.h"
#include "BswSrv_System.h"
#include "BswDrv_Usart.h"
#include "BswDrv_Debug.h"
#include "BswDrv_nfc.h"
#include "BswSrv_Aes.h"
#include "BswDrv_Rtc.h"
#include "BswDrv_sc8042b.h"


#define USER_NFC_CARD     			1

//主区域
#define SECTOR_MAIN					12
#define BLOCK_MAIN					49

//备用区域
#define	SECTOR_BAK					2
#define BLOCK_BAK					9


/*密码卡默认认证密码KEYA*/
static unsigned char SecretCardKEY_A[6] = {'6', 'f', '7', 'd', '2', 'k'};
/*AES加密密钥*/
static unsigned char SeedKeyA[16]="chargerlink1234";

/*读卡标志1-读卡 0-禁止读卡*/
static uint8_t readCardFlag = 1;
/*上次读到卡时间，5内不能重复读卡*/
static uint32_t readCardTime = 0;






//读卡处理
void Check_M1_Card(void)
{
	uint8_t PICC_ATQA[2],PICC_SAK[3],PICC_UID[4];
	
	if ((TypeA_CardActivate(PICC_ATQA,PICC_UID,PICC_SAK)==OK))
	{
		readCardFlag = 0;
		readCardTime = osGetTimes();
		// PrintfData("PICC_UID:",PICC_UID,4);
        // PrintfData("PICC_ATQA1:",PICC_ATQA,2);
		//密钥卡
		if(Mifare_Auth(0,2,SecretCardKEY_A,PICC_UID) == OK)
        {
			CL_LOG("this is SecretCard.\r\n");
			Sc8042bSpeech(VOIC_CARD);
		}
        else
		{	//非密钥卡
			uint8_t ucardkeyA[16]={0};
			AES_KEY aes;
			memset(&aes,0,sizeof(AES_KEY));
			AES_set_encrypt_key(SeedKeyA, 128, &aes);
            
			uint8_t card[16] ;
			memset(card,0,16);
			memcpy(card,PICC_UID,4);
			AES_encrypt(card, ucardkeyA, &aes);
            
			if((TypeA_CardActivate(PICC_ATQA,PICC_UID,PICC_SAK)==OK))
            {
				uint8_t result = 0;
				uint8_t block_data[16];
				memset((void*)block_data,0,16);
			    // PrintfData("PICC_ATQA2:",PICC_ATQA,2);
				//PrintfData("ucardkeyA=",ucardkeyA,16);

				//主扇区密码认证
				if(Mifare_Auth(0,SECTOR_MAIN,ucardkeyA,PICC_UID) == OK)
				{
					if(Mifare_Blockread(BLOCK_MAIN,block_data) == OK) 
					{
						result = 1;
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
								result = 1;
							}
						}
						else
						{
							CL_LOG("卡片密码认证失败...\r\n");
							Sc8042bSpeech(VOIC_CARD_INVALID);
						}
					}
			   }
				
				if(result == 1)
				{
					uint8_t cardId[16]={0,};
					sprintf((char*)cardId, "%x%02x%02x%02x%02x",block_data[4],block_data[5],block_data[6],block_data[7],block_data[8]);
					if(GlobalInfo.readCard_Callback)
					{
						GlobalInfo.readCard_Callback(3,cardId);
					}

				}
			}
		}
		TypeA_Halt();
	}
}

void ReadCardHandle(void)
{
	//读卡处理
	Check_M1_Card();
}


void NFCardTask(void)
{

#if USER_NFC_CARD

    uint32_t TimeTicks = 0;
    uint32_t authCardTicks = 0;

	if(BswDrv_FM175XX_Init() == CL_OK)
	{
		GlobalInfo.card_state = 1;
		//进入睡眠模式
		BswDrv_FM175XX_SetPowerDown(1);
		CL_LOG("FM175XX_Init OK.\r\n");
	}
	else
	{
		GlobalInfo.card_state = 0;
		CL_LOG("FM175XX_Init Error.\r\n");
	}
#endif

	while(1)
	{
		osDelay(100);

#if USER_NFC_CARD
		
		if(GlobalInfo.card_state == 0)/**读卡器检测**/
		{
			if(60000 <= (osGetTimes() - TimeTicks))
			{
				TimeTicks = osGetTimes();
				if(BswDrv_FM175XX_Init() == CL_OK)
				{
					GlobalInfo.card_state = 1;
					//进入睡眠模式
					BswDrv_FM175XX_SetPowerDown(1);
					CL_LOG("FM175XX_Init OK.\r\n");
				}
				else
				{
					BswDrv_FM175XX_Reset();
					CL_LOG("FM175XX_Init Error.\r\n");
				}
			}
		}
		else/**寻卡*/
		{
			//寻卡 300ms寻一次卡
			if (readCardFlag == 1 && (300 <= (osGetTimes() - authCardTicks)))
			{
				authCardTicks = osGetTimes();
				// Set_Rf(3);	//寻卡前打开天线
				if(BswDrv_FM175XX_SetPowerDown(0) == CL_OK)//退出睡眠模式
				{
					ReadCardHandle();
				}
				BswDrv_FM175XX_SetPowerDown(1);//进入睡眠模式
				// Set_Rf(0);//寻卡后关闭天线
			}
			//读到卡后，5s内不再寻卡
			if(osGetTimes() - readCardTime >= 3000)
			{
				readCardTime = osGetTimes();
				readCardFlag = 1;
			}

			//定时检测读取卡器是否正常 60s检测一次
			if(60000 <= (osGetTimes() - TimeTicks))
			{
				TimeTicks = osGetTimes();
				if(BswDrv_FM175XX_Check() != CL_OK)
				{
					GlobalInfo.card_state = 0;
					CL_LOG("读卡器检测失败，重启模块...\r\n");
				}
			}
		}
	#endif

	}
}


