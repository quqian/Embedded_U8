#include "NFCard.h"
#include "system.h"
#include "usart.h"
#include "fm175xx.h"
#include "m1_card.h"
#include "aes.h"
#include "rtc.h"


#define USER_NFC_CARD     			0


static unsigned char SecretCardKEY_A[6] = {'6', 'f', '7', 'd', '2', 'k'};
static unsigned char SeedKeyA[16]="chargerlink1234";

//∂¡ø®¥¶¿Ì
void Check_M1_Card()
{
#if 1
	uint8_t PICC_ATQA[2],PICC_SAK[3],PICC_UID[4];
	uint8_t pCardId[8];
	
	if (TypeA_CardActivate(PICC_ATQA,PICC_UID,PICC_SAK) == OK)
	{
		PrintfData("PICC_UID:",PICC_UID,4);
        PrintfData("PICC_ATQA1:",PICC_ATQA,2);
		//√‹‘øø®
		if(Mifare_Auth(0,2,SecretCardKEY_A,PICC_UID) == OK)
        {
			CL_LOG("this is SecretCard.\n");
			//Sc8042bSpeech(VOIC_CARD);
		}
        else
		{//∑«√‹‘øø®
			uint8_t ucardkeyA[16]={0};
            uint8_t card[16];
			AES_KEY aes;
			memset(&aes,0,sizeof(AES_KEY));
			AES_set_encrypt_key(SeedKeyA, 128, &aes);
            
			memset(card,0,16);
			memcpy(card,PICC_UID,4);
			AES_encrypt(card, ucardkeyA, &aes);
            //PrintfData("ˆƒ«∂Check_M", ucardkeyA, 16);
            
			if((TypeA_CardActivate(PICC_ATQA,PICC_UID,PICC_SAK)==OK))
            {
				uint8_t block_data[16];
				memset((void*)block_data,0,16);
			    PrintfData("PICC_ATQA2:",PICC_ATQA,2);
                
				if(Mifare_Auth(0,2,ucardkeyA,PICC_UID) == OK)
				{
					if(Mifare_Blockread(9,block_data) == OK) 
					{
						sprintf((char*)GlobalInfo.current_usr_card_id, "%02x%02x%02x%02x%02x",
								block_data[4],block_data[5],block_data[6],block_data[7],block_data[8]);
                        CL_LOG("block_data[4] = %02x ,block_data[5] = %02x ,block_data[6] = %02x ,block_data[7] = %02x ,block_data[8] = %02x .\n",
                            block_data[4],block_data[5],block_data[6],block_data[7],block_data[8]);
						
						StringToBCD(pCardId, (const char *)&GlobalInfo.current_usr_card_id[0]);
					    PrintfData("ø®∫≈", &pCardId[0], 5);

						
						memcpy((char*)GlobalInfo.current_usr_card_id, &block_data[4], 5);
					
						GlobalInfo.ReadCardFlag = 0xa5;
					}
				}
                else
				{
					CL_LOG("Mifare_Auth error.\n");
				}
			}
		}
		
		TypeA_Halt();
	}
#endif
}



void ReadCardHandle(void)
{
    //∂¡ø®¥¶¿Ì
    Check_M1_Card();
}

void NFCardTask(void)
{
    ReadCardHandle();
}


