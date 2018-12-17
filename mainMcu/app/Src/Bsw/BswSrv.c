#include "BswSrv.h"
#include "BswSrv_System.h"
#include "BswDrv_SPI_Fm175xx.h"
#include "BswDrv_IC_RF433.h"
#include "BswSrv_RF433.h"
#include "BswSrv_Air720.h"
#include "BswSrv_WG215.h"
#include "BswSrv_CardBoard.h"
#include "BswSrv_Key.h"


void BswSrv_Init(void)
{
	/*加载系统参数*/
	BswSrv_LoadSystemInfo();

	/*433模块服务接口初始化*/
	BswSrv_RF433_LoadDevInfo();

	/*4G模块初始化*/
	BswSrv_Air720_Init();

	/*wifi模块初始化*/
	BswSrv_WG215_Init();
	
	/*刷卡版*/
	BswSrv_CB_Init();

	BswSrv_Key_Init();
	
    /*打印重启标志*/
	BswSrv_SystemResetRecord();
}
