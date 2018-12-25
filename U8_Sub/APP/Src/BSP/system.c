#include "gd32f3x0.h"
#include "system.h"
#include "includes.h"
#include "flash.h"
#include "flash_usr.h"
#include "usart.h"
#include "gd32f3x0_rcu.h"
#include "gd32f3x0_fwdgt.h"
#include "gd32f3x0_timer.h"
#include "gd32f3x0_misc.h"
#include "gd32f3x0_gpio.h"


#define WATCH_DOG_ENABLE        	1

SYSTEM_INFO_T	SystemInfo = {0,};
GLOBAL_INFO_T	GlobalInfo = {0,};



int GetPktSum(uint8_t *pData, uint16_t len)
{
    int i;
    uint8_t sum = 0;

    for (i=0; i<len; i++) 
	{
        sum += pData[i];
    }
    return sum;
}

void WatchDogInit(void) 
{	
#if WATCH_DOG_ENABLE
    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait till IRC40K is ready */
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC40K));
 	/* confiure FWDGT counter clock: 40KHz(IRC40K) / 64 = 0.625 KHz */
    fwdgt_config(0xff0, FWDGT_PSC_DIV64);   //6S后会产生复位
    
    fwdgt_counter_reload();
    fwdgt_enable();
#endif
}

void FeedWatchDog(void)
{
#if WATCH_DOG_ENABLE
    fwdgt_counter_reload();
#endif
}

#define MAGIC_NUM_BASE                  0x123456AE
void LoadSystemInfo(void)
{
//    const uint8_t station_id[8] = {0x00,0x00,0x00,0x66,0x77,0x88,0x99,0x00};
    FlashReadSysInfo(&SystemInfo, sizeof(SystemInfo));
	memset((void*)&GlobalInfo, 0, sizeof(GlobalInfo));
	
    if ((MAGIC_NUM_BASE) == SystemInfo.magic_number) 
   // if(0)
    {
        printf("\n\n\n===========================================================\n");
        CL_LOG("\rU8Sub启动App\n");
    }
    else
    {
        printf("\n\n\n***********************************************************\n");
        CL_LOG("\rU8Sub初次启动App\n");
		SetRtcCount(1545649294);
//	time_t mktime(strcut tm * timeptr);
        memset((void*)&SystemInfo, 0, sizeof(SystemInfo));
        SystemInfo.magic_number = MAGIC_NUM_BASE;
//        memcpy(SystemInfo.stationId, station_id,sizeof(station_id));
        FlashWriteSysInfo(&SystemInfo, sizeof(SystemInfo));
    }
    CL_LOG("\r设备版本号: fw_version = %d, subVersion1 = %d, subVersion2 = %d. \n", FW_VERSION, FW_VERSION_SUB1, FW_VERSION_SUB2);
    CL_LOG("\n\r文件编译时间, 月日年 %s 时分秒%s \n", __DATE__, __TIME__);
}

void StartDelay(void)
{
    uint32_t i = 0;
    uint32_t k = 0;
    
    for(i = 0; i < 1; i++)
    {
        for(k = 0; k < 10000000; k++)
        {
            __NOP();
        }
    }
}

void SystemResetRecord(void)
{
    if(rcu_flag_get(RCU_FLAG_EPRST) == SET)
    {
        CL_LOG("系统外部复位管教复位\n");
    }
    else if(rcu_flag_get(RCU_FLAG_PORRST) == SET)
    {
        CL_LOG("系统上电复位\n");
    }
    else if(rcu_flag_get(RCU_FLAG_SWRST) == SET)
    {
        CL_LOG("系统软件复位\n");
    }
    else if(rcu_flag_get(RCU_FLAG_FWDGTRST) == SET)
    {
        CL_LOG("系统独立看门狗复位\n");
    }
    else if(rcu_flag_get(RCU_FLAG_WWDGTRST) == SET)
    {
        CL_LOG("系统窗口看门狗复位\n");
    }
    else if(rcu_flag_get(RCU_FLAG_LPRST) == SET)
    {
        CL_LOG("系统低功耗复位\n");
    }
    rcu_all_reset_flag_clear();
    
}


int MuxSempTake(uint8_t *pSemp)
{
//    while (0xb5 == *pSemp) 
//	{
//        FeedWatchDog();
//        vTaskDelay(4);
//    }
//    *pSemp = 0xb5;
    return CL_OK;
}


void MuxSempGive(uint8_t *pSemp)
{
    *pSemp = 0;
}


unsigned char Asc2Int(char ch) 
{
    unsigned char val = 0;
	
    if ((ch >= '0') && (ch <= '9')) 
	{
        val = ch - '0';
    } 
	else if ((ch >= 'A') && (ch <= 'F')) 
	{
        val = ch - 'A' + 10;
    } 
	else if ((ch >= 'a') && (ch <= 'f')) 
	{
        val = ch - 'a' + 10;
    }
	
    return val;
}

int StringToBCD(unsigned char *BCD, const char *str) 
{
    unsigned char chh, chl;
    int length = strlen(str);
    int index = 0;

    for (index = 0; index < length; index += 2)
    {
        chh = Asc2Int(str[index]);
        chl = Asc2Int(str[index + 1]);
       
        BCD[index / 2] = (chh << 4) | chl;
    }
    return (length / 2);
}

char *BCDToString(char *dest, unsigned char *BCD, int bytes) 
{
    char  temp[] = "0123456789ABCDEF";
    int index = 0;
    int length = 0;
    if (BCD == NULL || bytes <= 0)
        return NULL;
    
    for (index = 0; index < bytes; index++) {
        dest[length++] = temp[(BCD[index] >> 4) & 0x0F];
        dest[length++] = temp[BCD[index] & 0x0F];
    }
    dest[length] = '\0';
    return dest;
}

#define TIMER_MAX				2

rcu_periph_enum RCU_TIMERX[TIMER_MAX] = {RCU_TIMER0, RCU_TIMER1};
uint32_t TIMERX[TIMER_MAX] = 			{TIMER0, 		TIMER1};
uint8_t TIMERX_IRQ[TIMER_MAX] = 		{TIMER0_BRK_UP_TRG_COM_IRQn, TIMER1_IRQn};

/*****************************************************************************
** Function name:   	TimerConfig
** Descriptions:        configure the TIMER peripheral
** input parameters:    
** Returned value:	    None
** Author:              quqian
*****************************************************************************/
void TimerConfig(uint8_t Index, uint32_t Period, uint8_t UserIrq)
{
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMERX[Index]);
    timer_deinit(TIMERX[Index]);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 83;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = Period;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMERX[Index], &timer_initpara);

	if(1 == UserIrq)
	{
		timer_interrupt_enable(TIMERX[Index], TIMER_INT_UP | TIMER_FLAG_UP);
		timer_flag_clear(TIMERX[Index], TIMER_FLAG_UP);
		nvic_irq_enable(TIMERX_IRQ[Index], 1, 1);
	}
	
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMERX[Index]);
	timer_enable(TIMERX[Index]);
	if(0 == Index)
	{
		timer_disable(TIMERX[Index]);
	}
}
#if USE_TIMER1
__IO uint32_t gSystick = 0;
void TIMER1_IRQHandler(void)
{
#if 1
	if(RESET != timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP))
	{
		timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);	
        
        gSystick++;
        //gpio_bit_toggle(GPIOB, GPIO_PIN_1);
	}
#endif
}

uint32_t GetTimeTicks(void)
{
	return gSystick;
}
#endif

