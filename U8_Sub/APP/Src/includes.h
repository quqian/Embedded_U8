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



#define FW_VERSION                      0
#define FW_VERSION_SUB1                 3
#define FW_VERSION_SUB2                 1


#define FLASH_SIZE                      64     //设备flash大小256K
#define FLASH_PAGE_SIZE					1024

#define BOOT_SIZE	                	((uint32_t)0x2000)
#define AppFlashAddr	                (FLASH_BASE + BOOT_SIZE)		//App运行入口地址
#define AppUpBkpAddr					(AppFlashAddr + (uint32_t)27 * 1024)				//备份区

#define SystemInfoAddr 					(FLASH_BASE + ((uint32_t)62) * 1024)
#define UPGRADE_INFO					(FLASH_BASE + ((uint32_t)63) * 1024)				//升级信息
#define APP_FW_SIZE                     (AppUpBkpAddr - AppFlashAddr)


#define DEBUG_INDEX            			0
#define RS232_INDEX            			1


#define CL_OK                           0
#define CL_FAIL                         (-1)
#define CL_TRUE                         1
#define CL_FALSE                        0



#define Feed_WDT						FeedWatchDog




#define CL_LOG(fmt,args...) do {    \
    printf("[U8s %d] %s:(%d)" fmt, GetTimeTicks()/1000, __func__, __LINE__, ##args); \
}while(0)



#endif



