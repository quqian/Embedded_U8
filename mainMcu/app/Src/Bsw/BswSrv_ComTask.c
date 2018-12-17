/*
 * @Author: zhoumin 
 * @Date: 2018-10-26 19:13:14 
 * @def :串口通信任务-刷卡板读取/上位机配置
 * @Last Modified by: zhoumin
 * @Last Modified time: 2018-11-02 18:01:00
 */

#include "BswSrv_ComTask.h"
#include "BswDrv_Rtc.h"
#include "BswSrv_System.h"
#include "BswDrv_Usart.h"
#include "BswSrv_CardBoard.h"
#include "BswSrv_ProductTest.h"
#include "BswSrv_FwUpgrade.h"


void ComTask(void)
{
	uint32_t tick = 0;

  	ComRxSem = osSemaphoreCreate(NULL,10);

	while(1)
	{
		if(osOK == osSemaphoreWait(ComRxSem,1000))
		{
			BswSrv_CB_RecvData();		//接收刷卡版数据
			BswSrv_ProTest_RecvData();	//接收上位机数据
		}
		
		//按键板检测
		if((uint32_t)(osGetTimes() - GlobalInfo.lastRecvCBTime) > (1000*180)) 
		{
			CL_LOG("check CardBoard disconnect.\r\n");
			GlobalInfo.lastRecvCBTime = osGetTimes();
			GlobalInfo.CBInitOK = 0;
			//按键板重启
			BswSrv_CB_Reset();
		}

		//60分钟检测检测一次按键板是否有升级
		if(GlobalInfo.CBInitOK && (uint32_t)(osGetTimes() - tick) > (1000*3600))
		{
			tick = osGetTimes();
			if(BswSrv_Upgrade_ReadHeadInfo(FW_U8_BAK,NULL) == CL_OK)
			{
				CL_LOG("检测到刷卡板有更新..\r\n");
				BswSrv_StartCardBoard_UpgradeTask();
			}
		}
	}
}





