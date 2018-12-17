#include "APP.h"
#include "BswSrv_System.h"
#include "App_Main.h"
#include "BswDrv_GPIO.h"
#include "BswDrv_Adc.h"
#include "App_ProductTest.h"
#include "App_NetProto.h"

void APP_Init(void)
{
    GlobalInfo.readCard_Callback = App_ReadCard_CallBack;
	
	memset(&cardAuthorMsg,0,sizeof(cardAuthorMsg));
	
    BswDrv_ADC_Start();

    App_ProTest_Init();

    LED_R_ON();
}

