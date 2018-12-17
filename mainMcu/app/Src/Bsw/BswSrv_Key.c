/*
 * @Author: zhoumin 
 * @Date: 2018-10-18 16:15:28 
 * @Last Modified by: zhoumin
 * @Last Modified time: 2018-10-28 22:42:57
 */
#include "includes.h"
#include "BswSrv_Key.h"
#include "BswDrv_IIC.h"
#include "BswDrv_Debug.h"
#include "BswDrv_GPIO.h"
#include "BswDrv_sc8042b.h"

#define KEY_POWER_ON()  gpio_bit_reset(GPIOB,GPIO_PIN_12)

static void BswSrv_Key_WritePararm(void);

static __IO uint8_t keyFlag = 0;


__weak void BswSrv_Key_Values(uint8_t key)
{
    UNUSED(key);
}

void EXTI4_IRQHandler(void)
{
	if (RESET != exti_interrupt_flag_get(EXTI_4)) {
        exti_interrupt_flag_clear(EXTI_4);

        //µÍµçÆ½´¥·¢
        if(READ_TP_IN() == 0)
        {
            keyFlag = 1;
        }
    }
}




void BswSrv_Key_Loop(void)
{
    if(keyFlag == 1)
    {
		uint8_t keyBuf[6] = {0};
        int keyValues = CL_FAIL;
        if(BswDrv_IIC_ReadData(SIM_IIC,R_ADDR,keyBuf,6) != CL_FAIL)
        {
            // PrintfData("keyBuf:",keyBuf,6);
            if ((keyBuf[0] >> 7) == 1) {
                
                if((keyBuf[1] & KEY_TP0) != 0)
                {
                    keyValues = 1;
                }
                else if((keyBuf[1] & KEY_TP1) != 0)
                {
                    keyValues = 2;
                }
                else if((keyBuf[1] & KEY_TP2) != 0)
                {
                    keyValues = 3;
                }
                else if((keyBuf[1] & KEY_TP3) != 0)
                {
                    keyValues = 4;
                }
                else if((keyBuf[1] & KEY_TP4) != 0)
                {
                    keyValues = 5;
                }
                else if((keyBuf[1] & KEY_TP5) != 0)
                {
                    keyValues = 6;
                }
                else if((keyBuf[1] & KEY_TP6) != 0)
                {
                    keyValues = 7;
                }
                else if((keyBuf[1] & KEY_TP7) != 0)
                {
                    keyValues = 8;
                }
                else if((keyBuf[2] & KEY_TP8) != 0)
                {
                    keyValues = 9;
                }
                else if((keyBuf[2] & KEY_TP9) != 0)
                {
                    keyValues = 10;
                }
                else if((keyBuf[2] & KEY_TP10) != 0)
                {
                    keyValues = 11;
                }
                else if((keyBuf[2] & KEY_TP11) != 0)
                {
                    keyValues = 12;
                }
                else
                {
                    keyValues = CL_FAIL;
                }
                if(keyValues != CL_FAIL)
                {
                    Sc8042bSpeech(VOIC_KEY);
                    BswSrv_Key_Values((uint8_t)keyValues);
                }
            }
        }
		
		keyFlag = 0;
    }
}




void BswSrv_Key_WritePararm(void)
{
	uint8_t keyBuf[4] = {0xB1,0x83,0x00,0x00};
	
	BswDrv_IIC_WritedData(SIM_IIC,W_ADDR,keyBuf,4);
}


void BswSrv_Key_Init(void)
{
    KEY_POWER_ON();
    // osDelay(100);
    BswSrv_Key_WritePararm();
}
