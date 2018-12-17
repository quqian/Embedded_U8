#ifndef __INCLUDES_H__
#define __INCLUDES_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include "gd32f30x.h"


#define FW_VERSION                      1


#define FLASH_SIZE                      0x40000     //??flash??256K
#define FLASH_PAGE_SIZE					2048


//0x08000000
#define BOOT_SIZE	                	((uint32_t)0x3000)                  //12K
#define AppFlashAddr	                (FLASH_BASE + BOOT_SIZE)		    //App运行入口地址 (100k) 
#define AppUpBkpAddr	                (AppFlashAddr + (uint32_t)0x19000)	//app缓存数据存储基地址(132k 主板+副板)
#define SystemInfoAddr                  (AppUpBkpAddr + (uint32_t)0x21000)  //系统配置信息(2K)
#define SysUpInfoAddr	                (SystemInfoAddr + (uint32_t)0x800)  //升级请求头消息(2K)
#define OtherInfoAddr                  	(SysUpInfoAddr + (uint32_t)0x800)   //其他(8K)

#define APP_FW_SIZE                     (AppUpBkpAddr - AppFlashAddr)


#define CL_OK                           0
#define CL_FAIL                         (-1)
#define CL_TRUE                         1
#define CL_FALSE                        0



#endif



