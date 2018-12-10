#ifndef __INCLUDES_H__
#define __INCLUDES_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include "gd32f3x0.h"
#include "gd32f3x0_rcu.h"
#include "gd32f3x0_gpio.h"
#include "gd32f3x0_misc.h"
#include "gd32f3x0_fmc.h"
#include "gd32f3x0_fmc.h"
#include "system.h"
#include "systick.h"
#include "ProtoWithMainBoard.h"
#include "sc8042b.h"




//#if 0
//	//#define NET_SERVER_IP                   "172.16.0.228"     //新开发环境
//	#define NET_SERVER_IP                   "47.97.238.64"     //新开发环境
//	#define NET_SERVER_PORT                 "10010"
//#elif 0
//	#define NET_SERVER_IP                   "47.97.224.240"     //正式环境
//	#define NET_SERVER_PORT                 "10010"
//#elif 0
//	#define NET_SERVER_IP                   "47.97.236.187"		//预发布环境
//	#define NET_SERVER_PORT                 "10010"
//#else
//	#define NET_SERVER_IP                   "test.access.chargerlink.com"	//测试环境
//	#define NET_SERVER_PORT                 "10010"
////	#define NET_SERVER_IP                   "192.168.20.215"	//wifi模拟测试
////	#define NET_SERVER_PORT                 "10010"
//#endif

#define FW_VERSION                      0
#define FW_VERSION_SUB1                 1
#define FW_VERSION_SUB2                 1


#define FLASH_SIZE                      128     //设备flash大小128K
#define FLASH_PAGE_SIZE					1024

#define BOOT_SIZE	                	((uint32_t)0x2000)
#define AppFlashAddr	                (FLASH_BASE + BOOT_SIZE)		//App运行入口地址
#define AppUpBkpAddr					(AppFlashAddr + ((uint32_t)56) * 1024)	//应用区		56K

#define SystemInfoAddr 					(AppUpBkpAddr + ((uint32_t)56) * 1024)	//备份区		56K	//(FLASH_BASE + ((uint32_t)124) * 1024)
#define UPGRADE_INFO					(FLASH_BASE + ((uint32_t)127) * 1024)				//升级信息
#define APP_FW_SIZE                     (AppUpBkpAddr - AppFlashAddr)


#define DEBUG_INDEX            			0
#define RS232_INDEX            			1


#define CL_OK                           0
#define CL_FAIL                         (-1)
#define CL_TRUE                         1
#define CL_FALSE                        0

#define STATION_MACHINE_TYPE            "R6"
#define CHARGER_TYPE                    3  //0:无效；1:X9；2:X10；3:R6；4:X6P；5:X6L；6:R8
#define REGISTER_CODE					"572059137094"
#define CHARGER_SN_LEN			        8   //


//#define osGetTimes()                    (xTaskGetTickCount() * portTICK_PERIOD_MS)
//#define GetRtcCount 	                GetTimeStamp /*(xTaskGetTickCount()*portTICK_PERIOD_MS/1000)*/
#define Feed_WDT						FeedWatchDog




#define CL_LOG(fmt,args...) do {    \
    printf("[U8s] %s:(%d)" fmt, __func__, __LINE__, ##args); \
}while(0)

//extern char* GetCurrentTime(void);
//#define CL_LOG(fmt,args...) do {    \
//    printf("[U8sub %s] %s:(%d)" fmt, GetCurrentTime(), __func__, __LINE__, ##args); \
//}while(0)

//#define PRINT_LOG(fmt,args...) do \
//{    \
//    printf("[%s]"fmt,GetCurrentTime(), ##args); \
//}while(0)

#endif



