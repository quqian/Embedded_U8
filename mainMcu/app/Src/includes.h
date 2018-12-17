#ifndef __INCLUDES_H__
#define __INCLUDES_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include "gd32f30x.h"
#include "cmsis_os.h"


/**
 * V1:第一次发版
 * V2:卡号上报有空格   远程控制指令响应包序号和接收的序号不一致
 * V3:修改bug：WiFi发送数据失败主板重启
 * V4:固件升级判断版本号，版本号一致不升级    与上位机通信串口波特率改成19200，和其他设备保持一致   增加功能：使用上位机升级固件
 * V5: wifi下载固件流程优化。正式版本发布
 * V6：修改bug：出门刷卡标志不对   完成产测软件
 * V7: FM1752读卡器器增加低功耗  产测软件测试
 * V8:433测试优化  修改主板和刷卡版之间的协议  优化4G拨号,增加专网
 * V9：产测软件增加门锁状态检测,修改注册码设置格式
 * V10：
 * V11：
 **/


//与服务器同步版本
#define VERSON_SER						12

//发布版本
#define FW_VERSION                      0
#define FW_VERSION_SUB1                 ((VERSON_SER/10)%10)
#define FW_VERSION_SUB2                 (VERSON_SER%10)

#define 	SPECIAL_NET_CARD			0   //0-公网  1-专网
#define     ID2                         0	//0-非加密  1-加密
#define     ENVI_TYPE                   1 	//0-正式 1-测试  2-预发布  3-开发

#if SPECIAL_NET_CARD == 0
	#if ENVI_TYPE == 0						//正式环境
		#define NET_SERVER_IP                   "access.sharecharger.com"
		#define NET_SERVER_PORT                 10010
		#define REGISTER_CODE                   "016987547845"
	#elif ENVI_TYPE == 1 					//测试 
		#define NET_SERVER_IP                   "test.access.chargerlink.com"
		#define NET_SERVER_PORT                 10010
		#define REGISTER_CODE                   "016987547845"
	#elif ENVI_TYPE == 2					//预发布
		#define NET_SERVER_IP                   "47.97.236.187"
		#define NET_SERVER_PORT                 10010
		#define REGISTER_CODE                   "016987547845"
	#elif ENVI_TYPE == 3					//开发
		#define NET_SERVER_IP                   "47.97.238.64"
		#define NET_SERVER_PORT                 10010
		#define REGISTER_CODE                   "016987547845"
	#endif
#elif SPECIAL_NET_CARD == 1
	#if ENVI_TYPE == 0						//正式环境
		#define NET_SERVER_IP                   "www.access.com"    //正式环境
		#define NET_SERVER_PORT                 10010
		#define REGISTER_CODE                   "016987547845"
	#elif ENVI_TYPE == 1 					//测试
		#define NET_SERVER_IP                   "test.access.com"
		#define NET_SERVER_PORT                 10010
		#define REGISTER_CODE                   "016987547845"
	#elif ENVI_TYPE == 2					//预发布
		#define NET_SERVER_IP                   "172.16.0.245"
		#define NET_SERVER_PORT                 10010
		#define REGISTER_CODE                   "016987547845"
	#elif ENVI_TYPE == 3					//开发
		#define NET_SERVER_IP                   "172.16.0.228"
		#define NET_SERVER_PORT                 10010
		#define REGISTER_CODE                   "016987547845"
	#endif
#endif

#define DEVICE_TYPE						14	//设备类型 -- U8
#define STATION_MACHINE_TYPE            "U8"


#define FLASH_SIZE                      0x40000     //设备flash大小256K
#define FLASH_PAGE_SIZE					2048
//0x08000000
#define BOOT_SIZE	                	((uint32_t)0x3000)                  //12K
#define AppFlashAddr	                (FLASH_BASE + BOOT_SIZE)		    //App运行入口地址 (100k) 
#define AppUpBkpAddr	                (AppFlashAddr + (uint32_t)0x19000)	//app缓存数据存储基地址(132k 主板+副板) 801C000
#define SystemInfoAddr                  (AppUpBkpAddr + (uint32_t)0x21000)  //系统配置信息(2K)  
#define SysUpInfoAddr	                (SystemInfoAddr + (uint32_t)0x800)  //升级请求头消息(2K)
#define OtherInfoAddr                  	(SysUpInfoAddr + (uint32_t)0x800)   //其他(8K)

#define APP_FW_SIZE                     (AppUpBkpAddr - AppFlashAddr)


#define CL_OK                           0
#define CL_FAIL                         (-1)
#define CL_TRUE                         1
#define CL_FALSE                        0


#define RF_DEV_MAX						20 //烟感设备最大绑定数量

#define TX_FAIL_MAX_CNT	                3

#define osGetTimes                      osKernelSysTick
#define GetRtcCount 	                GetTimeStamp /*(xTaskGetTickCount()*portTICK_PERIOD_MS/1000)*/
#define UNUSED(X) (void)X


extern char* GetCurrentTime(void);
#define CL_LOG(fmt,args...) do {    \
    printf("%s|[U8]:%s (%d)" fmt, GetCurrentTime(), __func__, __LINE__, ##args); \
}while(0)

#define PRINT_LOG(fmt,args...) do \
{    \
    printf("[%s]"fmt,GetCurrentTime(), ##args); \
}while(0)

#endif



