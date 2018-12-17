/*
 * @Author: zhoumin 
 * @Date: 2018-10-12 14:17:00 
 * @Last Modified by: zhoumin
 * @Last Modified time: 2018-11-08 09:59:39
 */
#include "includes.h"
#include "BswSrv.h"
#include "BswDrv.h"
#include "APP.h"
#include "App_Main.h"
#include "BswSrv_ComTask.h"
#include "BswSrv_NFCard_Task.h"
#include "BswSrv_NetTask.h"
#include "BswSrv_WifiBlueTask.h"


#define VECT_TAB_OFFSET  BOOT_SIZE	//中断向量表偏移地址


//开始任务任务函数
void start_task(void *pvParameters)
{
	xTaskCreate((TaskFunction_t)App_MainTask,   "MainTask", 	300, NULL, 3, NULL);
	xTaskCreate((TaskFunction_t)SurfNet_Task,   "SurfNet_Task", 600, NULL, 3, NULL);
	xTaskCreate((TaskFunction_t)NFCardTask,	    "NFCardTask",	256, NULL, 4, NULL);   
	xTaskCreate((TaskFunction_t)WifiBlueTask,	"BlueTask", 	600, NULL, 2, NULL);
	xTaskCreate((TaskFunction_t)ComTask,		"ComTask", 	    384, NULL, 4, NULL);
	
	vTaskDelete(NULL);
}


int main(void)
{	
	/* 设置中断向量表偏移地址 */
	nvic_vector_table_set(NVIC_VECTTAB_FLASH,VECT_TAB_OFFSET);
	
	/*驱动层初始化*/
	BswDrv_Init();
	
	/*服务层初始化*/
	BswSrv_Init();
	
	/*应用层初始化*/
	APP_Init();

	//创建开始任务
	xTaskCreate((TaskFunction_t)start_task, "start_task", 128, NULL, 1, NULL);  

	vTaskStartScheduler();          //开启任务调度

	for(;;);
}





