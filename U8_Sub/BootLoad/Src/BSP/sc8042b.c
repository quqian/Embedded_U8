
#include "sc8042b.h"
#include "systick.h"


void Delay_100uSec(int count)
{
    uint16_t i;
	
    for (i = 0; i < 192; i++)
    {
		__NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
}

#define OS_DELAY_MS					DelayMsWithNoneOs
#define Sc8042bDelayUs				DelayUs//Delay_100uSec//

int PlayVoice(uint8_t cnt)
{
	uint8_t i = 0;
    int count = 0;
	
    //判忙  2s超时
    while(READ_AU_BUSY() != 0)
	{
        OS_DELAY_MS(10);
        if(count++ > 200)
		{
            return -1;
        }
    }

    AU_RST_EN();
	Sc8042bDelayUs(100);
	AU_RST_DIS();
	Sc8042bDelayUs(100);
	
    for(i = 0; i < cnt; i++)
	{
        Feed_WDT();
		
        AU_DATA_HIHG();
        Sc8042bDelayUs(100);
		AU_DATA_LOW();
        Sc8042bDelayUs(100);
    }
	
    return 0;
}


//提示操作失败及具体代码
void OptFailNotice(int code)
{
    uint8_t codeVal[3];

    Feed_WDT();
    CL_LOG("error code=%03d.\n",code);
    codeVal[0] = code / 100 % 10;
    codeVal[1] = code / 10 % 10;
    codeVal[2] = code / 1 % 10;
    Sc8042bSpeech(codeVal[0]+31);
    OS_DELAY_MS(250);
    Sc8042bSpeech(codeVal[1]+31);
    OS_DELAY_MS(250);
    Sc8042bSpeech(codeVal[2]+31);
    OS_DELAY_MS(250);
}


//提示操作成功及具体代码
void OptSuccessNotice(int code)
{
    uint8_t codeVal[3];

    Feed_WDT();
    CL_LOG("success code=%03d.\n",code);
    codeVal[0] = code / 100 % 10;
    codeVal[1] = code / 10 % 10;
    codeVal[2] = code / 1 % 10;
    Sc8042bSpeech(VOIC_SUCCESS);
    OS_DELAY_MS(1000);
    Sc8042bSpeech(codeVal[0]+31);
    OS_DELAY_MS(250);
    Sc8042bSpeech(codeVal[1]+31);
    OS_DELAY_MS(250);
    Sc8042bSpeech(codeVal[2]+31);
    OS_DELAY_MS(250);
}


void TempNotice(uint16_t temp)
{
    uint8_t codeVal[3];
    Feed_WDT();
    CL_LOG("temp=%d.\n",temp);
    codeVal[0] = temp / 100 % 10;
    codeVal[1] = temp / 10 % 10;
    codeVal[2] = temp / 1 % 10;
    Sc8042bSpeech(VOIC_TEM);
    OS_DELAY_MS(1000);
    Sc8042bSpeech(codeVal[0]+31);
    OS_DELAY_MS(250);
    Sc8042bSpeech(codeVal[1]+31);
    OS_DELAY_MS(250);
    Sc8042bSpeech(codeVal[2]+31);
    Sc8042bSpeech(VOIC_DEGREE);
    OS_DELAY_MS(250);
}


void CardBalanceSpeech(uint32_t balance)
{
	uint8_t tmp=0;

	if (balance>99999) 
	{
		return;
	}
	//Sc8042bSpeech(VOIC_CARD_BANLANCE);//卡片余额
	//OS_DELAY_MS(1000);
	tmp = balance / 10000;
	if (tmp != 0)
	{
		Sc8042bSpeech(tmp+31);
		OS_DELAY_MS(250);
		Sc8042bSpeech(VOIC_HUNDRED);//百
		OS_DELAY_MS(250);
		tmp = (balance/1000)%10;
		if (tmp != 0)
        {
			Sc8042bSpeech(tmp+31);
			OS_DELAY_MS(250);
			Sc8042bSpeech(VOIC_TEN);//十
			OS_DELAY_MS(250);
			tmp = (balance/100)%10;
			if(tmp != 0)
			{
				Sc8042bSpeech(tmp+31);
				OS_DELAY_MS(250);
			}
		} 
		else 
		{
			tmp = (balance/100)%10;
			if(tmp != 0) 
			{
				Sc8042bSpeech(31);
				OS_DELAY_MS(250);
				Sc8042bSpeech(tmp+31);
				OS_DELAY_MS(250);
			}
		}

		tmp = (balance/10)%10;
		if (tmp!=0) 
		{
			Sc8042bSpeech(VOIC_POINT);//点
			OS_DELAY_MS(250);
			Sc8042bSpeech(tmp+31);
			OS_DELAY_MS(250);
		}
		Sc8042bSpeech(VOIC_YUAN);//元
	} 
	else 
	{
		tmp = balance/1000;
		if (tmp != 0) 
		{
			if (tmp != 1) 
			{
				Sc8042bSpeech(tmp+31);
				OS_DELAY_MS(250);
			}
			Sc8042bSpeech(VOIC_TEN);
			OS_DELAY_MS(250);
			tmp = (balance/100)%10;
			if(tmp != 0) 
			{
				Sc8042bSpeech(tmp+31);
				OS_DELAY_MS(250);
			}
			tmp = (balance/10)%10;
			if (tmp!=0) 
			{
				Sc8042bSpeech(VOIC_POINT);
				OS_DELAY_MS(250);
				Sc8042bSpeech(tmp+31);
				OS_DELAY_MS(250);
			}
			Sc8042bSpeech(VOIC_YUAN);
		} 
		else 
		{
			tmp = balance/100;
			Sc8042bSpeech(tmp+31);
			OS_DELAY_MS(250);
			tmp = (balance/10)%10;
			if (tmp!=0) 
			{
				Sc8042bSpeech(VOIC_POINT);
				OS_DELAY_MS(250);
				Sc8042bSpeech(tmp+31);
				OS_DELAY_MS(250);
			}
			Sc8042bSpeech(VOIC_YUAN);
		}
	}
}

void SpeechCardBalance(uint32_t balance)
{
	uint8_t tmp=0;
	
	if (balance>999999) 
	{
		return;
	}
	
	Sc8042bSpeech(VOIC_CARD_BANLANCE);//卡片余额
	OS_DELAY_MS(1000);
	tmp = balance / 100000;
	if(tmp != 0)
	{
		Sc8042bSpeech(tmp+31);
		OS_DELAY_MS(250);
		Sc8042bSpeech(VOIC_THOUSAND);//千
		OS_DELAY_MS(250);
		tmp = (balance / 10000)%10;
		if(tmp != 0)
		{
			Sc8042bSpeech(tmp+31);
			OS_DELAY_MS(250);
			Sc8042bSpeech(VOIC_HUNDRED);//百
			OS_DELAY_MS(250);
			tmp = (balance/1000)%10;
			if (tmp != 0)
			{
				Sc8042bSpeech(tmp+31);
				OS_DELAY_MS(250);
				Sc8042bSpeech(VOIC_TEN);//十
				OS_DELAY_MS(250);
				tmp = (balance/100)%10;
				if(tmp != 0)
				{
					Sc8042bSpeech(tmp+31);
					OS_DELAY_MS(250);
				}
			} 
			else
			{
				tmp = (balance/100)%10;
				if(tmp != 0)
				{
					Sc8042bSpeech(VOIC_0);
					OS_DELAY_MS(250);
//					Sc8042bSpeech(31);
//					OS_DELAY_MS(250);
					Sc8042bSpeech(tmp+31);
					OS_DELAY_MS(250);
				}
			}

			tmp = (balance/10)%10;
			if (tmp!=0)
			{
				Sc8042bSpeech(VOIC_POINT);//点
				OS_DELAY_MS(250);
				Sc8042bSpeech(tmp+31);
				OS_DELAY_MS(250);
			}
			Sc8042bSpeech(VOIC_YUAN);//元
		} 
		else 
		{
			tmp = (balance % 100000)/1000;
			if (tmp != 0)
			{
				Sc8042bSpeech(VOIC_0);
				OS_DELAY_MS(250);
				Sc8042bSpeech(tmp+31);
				OS_DELAY_MS(250);
				Sc8042bSpeech(VOIC_TEN);	//十
				OS_DELAY_MS(250);
				tmp = (balance/100)%10;
				if(tmp != 0)
				{
					Sc8042bSpeech(tmp+31);
					OS_DELAY_MS(250);
				}
				tmp = (balance/10)%10;
				if (tmp!=0)
				{
					Sc8042bSpeech(VOIC_POINT);
					OS_DELAY_MS(250);
					Sc8042bSpeech(tmp+31);
					OS_DELAY_MS(250);
				}
				Sc8042bSpeech(VOIC_YUAN);
			} 
			else
			{
				tmp = (balance % 100000)/100;
				if (tmp != 0)
				{
					Sc8042bSpeech(VOIC_0);
					OS_DELAY_MS(250);
					Sc8042bSpeech(tmp+31);
					OS_DELAY_MS(250);
				}
				tmp = (balance/10)%10;
				if (tmp != 0)
				{
					Sc8042bSpeech(VOIC_POINT);
					OS_DELAY_MS(250);
					Sc8042bSpeech(tmp+31);
					OS_DELAY_MS(250);
				}
				Sc8042bSpeech(VOIC_YUAN);
			}
		}
	}
	else
	{
		CardBalanceSpeech(balance);
	}
}
//Sc8042bSpeech(VOIC_0);

#if 0
void SpeechStartCharge(void)
{
	uint8_t tmp=0;
	
	uint8_t curr_gunid = gChgInfo.current_usr_gun_id + system_info.baseGunSn - 1;
	Sc8042bSpeech(VOIC_NIN_YI_XUANZE);
	
	OS_DELAY_MS(800);
	tmp = curr_gunid / 10;
	if (tmp==0) 
	{
		Sc8042bSpeech(curr_gunid+31);
		OS_DELAY_MS(250);
	} 
	else 
	{
		Sc8042bSpeech(VOIC_TEN);
		OS_DELAY_MS(250);
		tmp = curr_gunid % 10;
		if (tmp != 0) 
		{
			Sc8042bSpeech(tmp+31);
			OS_DELAY_MS(250);
		}
	}
	Sc8042bSpeech(VOIC_HAO_CHAZUO);
	OS_DELAY_MS(800);
	Sc8042bSpeech(VOIC_CHARGER_MONEY);
	OS_DELAY_MS(800);
	Sc8042bSpeech(gChgInfo.money + 31);
	OS_DELAY_MS(250);
	Sc8042bSpeech(VOIC_YUAN);
	OS_DELAY_MS(250);
	Sc8042bSpeech(VOIC_VERIFIED_PLUG);
	OS_DELAY_MS(2000);
}
#endif

void SpeechChargeing(uint8_t id)
{
	uint8_t tmp=0;
	tmp = id / 10;
	
	if (tmp==0) 
	{
		tmp = id%10;
		Sc8042bSpeech(tmp+31);
		OS_DELAY_MS(250);
	} 
	else 
	{
		Sc8042bSpeech(VOIC_TEN);
		OS_DELAY_MS(250);
		tmp = id % 10;
		if (tmp != 0) 
		{
			Sc8042bSpeech(tmp+31);
			OS_DELAY_MS(250);
		}
	}
	Sc8042bSpeech(VOIC_HAO_CHAZUO);
	OS_DELAY_MS(800);
	Sc8042bSpeech(VOIC_START_CHARGING);
	OS_DELAY_MS(1000);
	Sc8042bSpeech(VOIC_STOP_CHARGER_TIP);
}


void SC8042B_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	//PB0--power
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    //PB4--AU_DATA
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_4);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    //PB3--AU_RST
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
   //PB5--AU_BUSY
	gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_5);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);

    AU_POWER_EN();
    AU_DATA_LOW();
    AU_RST_DIS();
}


