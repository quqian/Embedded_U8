#include "includes.h"
#include "App_NetProto.h"

static void App_NetFrameHandle(uint8_t type,uint8_t *data,uint16_t len);

/**
 *	WIFI接收数据回调函数
 */
void BswSrv_WIFI_RxData_Callback(uint8_t *data,uint16_t len)
{
	App_NetFrameHandle(1,data,len);
}


/**
 *	GPRS接收数据回调函数
 */
void BswSrv_GPRS_RxData_Callback(uint8_t *data,uint16_t len)
{
	App_NetFrameHandle(2,data,len);
}



/**
 *	网络数据协议处理
 */
void App_NetFrameHandle(uint8_t type,uint8_t *data,uint16_t len)
{

}


