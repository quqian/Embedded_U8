#include "includes.h"
#include "BswSrv_System.h"
#include "App_BlueProto.h"
#include "BswSrv_WG215.h"
#include "BswDrv_sc8042b.h"
#include "BswDrv_Rtc.h"
#include "BswSrv_FlashUsr.h"

static void App_BlueShakeReqProc(BLUE_PROTO_STR *pMsg);
static void App_BlueSendHeartBeatACK(BLUE_PROTO_STR *pMsg);
static void App_BlueReqBreak(BLUE_PROTO_STR *pMsg);
static void App_BlueSetChargerSn(BLUE_PROTO_STR *pMsg);
static void App_BlueSetChargerICode(BLUE_PROTO_STR *pMsg);
static void App_BlueRemoteCtrl(BLUE_PROTO_STR *pMsg);
static void App_BlueFWUpgradeNotice(BLUE_PROTO_STR *pMsg);
static void App_BlueFWDownLoad(BLUE_PROTO_STR *pMsg);
static void App_BlueSetWifiInfoProc(BLUE_PROTO_STR *pMsg);

static void App_HandleBlueFrame(uint8_t *data,uint16_t len);
static int CheckBluePkt(BLUE_PROTO_STR *pbuff, uint16_t len);


void BswSrv_Blue_RxData_Callback(uint8_t *data,uint16_t len)
{
    App_HandleBlueFrame(data,len);
}

void BswSrv_Blue_ConnectState_Callback(uint8_t state)
{
	CL_LOG("blue connect state = %d\r\n",state);
	if(state == 1)
	{
		GlobalInfo.blueLastOnlineTime = osGetTimes();
		GlobalInfo.isBlueConnect = 1;
	}
	else
	{
		if(GlobalInfo.isBlueConnect != 0)
		{
			BswSrv_Blue_Disconnent();
			GlobalInfo.isBlueConnect = 0;
			GlobalInfo.isBlueLogin = 0;
		}
	}
}

void App_Blue_SendData(BLUE_PROTO_STR *pbuff,uint16_t len)
{
	pbuff->head.start = 0xEE;
	pbuff->head.len = len;

	pbuff->data[len] = BswSrv_Tool_CheckSum((uint8_t*)pbuff,sizeof(BLUE_PROTO_HEAD_STR));

	BswSrv_Bule_SendData((void*)pbuff,sizeof(BLUE_PROTO_HEAD_STR)+len+1);
}


int CheckBluePkt(BLUE_PROTO_STR *pbuff, uint16_t len)
{
    uint8_t  sum;
    if (0xEE != pbuff->head.start) {
        return CL_FAIL;
    }
    sum = BswSrv_Tool_CheckSum((void*)pbuff, len-1);

    if (sum == pbuff->data[pbuff->head.len]) {
        return CL_OK;
    }
	CL_LOG("Blue msg check error.\n");
    return CL_FAIL;
}



void App_BlueShakeReqProc(BLUE_PROTO_STR *pMsg)
{
	BLUE_SHAKE_REQ_STR *shakeReq = (void*)pMsg->data;
	if(shakeReq->time != 0)
	{
		int cnt;
		time_t now = GetRtcCount();
		cnt = (now > shakeReq->time) ? now - shakeReq->time : shakeReq->time - now;
		
		if ((30 < cnt)) {
			//系统对时
			SetRtcCount(shakeReq->time+1);
			CL_LOG("set rtc time.\n");
		 }
	 }
	//是否记录手机标识码
	memcpy(GlobalInfo.phonesSn, shakeReq->phonesSn, sizeof(shakeReq->phonesSn));
	CL_LOG("phonesSn:%s Time=%d\n",shakeReq->phonesSn,shakeReq->time);

	GlobalInfo.blueLastOnlineTime = osGetTimes();
	GlobalInfo.isBlueConnect = 1;
	GlobalInfo.isBlueLogin = 1;

	Sc8042bSpeech(VOIC_BLUETOOTH_ONLINE);

	//ack
	BLUE_SHAKE_ACK_STR *shakeAck = (void*)pMsg->data;
	memset(shakeAck,0,sizeof(BLUE_SHAKE_ACK_STR));
	memcpy(shakeAck->name, STATION_MACHINE_TYPE, strlen(STATION_MACHINE_TYPE));
	shakeAck->fwVer = VERSON_SER;
	memcpy(shakeAck->chargerSn, &SystemInfo.stationId[3], sizeof(shakeAck->chargerSn));
	shakeAck->startNo = 0;
	shakeAck->portCnt = 0;
	App_Blue_SendData(pMsg,sizeof(BLUE_SHAKE_ACK_STR));
}


void App_BlueSendHeartBeatACK(BLUE_PROTO_STR *pMsg)
{
	BLUE_HEART_BEAT_ACK_STR *pACK = (BLUE_HEART_BEAT_ACK_STR*)pMsg->data;

	if(pACK->result == 0){
		GlobalInfo.blueLastOnlineTime = osGetTimes();
		if(pACK->time != 0)
		{
			int cnt;
			time_t now = GetRtcCount();
			cnt = (now > pACK->time) ? now - pACK->time : pACK->time - now;
			
			if ((30 < cnt)) {
				//系统对时
				SetRtcCount(pACK->time+1);
				CL_LOG("set rtc time.\n");
			 }
		 }
	}
}

void App_BlueReqBreak(BLUE_PROTO_STR *pMsg)
{
	//ACK
	BLUE_DISCONNECT_DEV_ACK_STR *pAck = (void*)pMsg->data;
	memset(pAck,0,sizeof(BLUE_DISCONNECT_DEV_ACK_STR));
	pAck->status = 0;

	App_Blue_SendData(pMsg,sizeof(BLUE_DISCONNECT_DEV_ACK_STR));

	osDelay(500);
	//断开蓝牙链接
	BswSrv_Blue_Disconnent();

	GlobalInfo.isBlueConnect = 0;
	GlobalInfo.isBlueLogin = 0;
	Sc8042bSpeech(VOIC_BLUETOOTH_OFFLINE);
}


void App_BlueSetChargerSn(BLUE_PROTO_STR *pMsg)
{
	//
}

void App_BlueSetChargerICode(BLUE_PROTO_STR *pMsg)
{
	//
}

void App_BlueRemoteCtrl(BLUE_PROTO_STR *pMsg)
{
	//
}


void App_BlueFWUpgradeNotice(BLUE_PROTO_STR *pMsg)
{
	uint8_t result = 0;
	//BLUE_DOWN_FW_REQ_STR *pFW = (void*)pMsg->data;


	//ack
	BLUE_DOWN_FW_ACK_STR *pACK = (void*)pMsg->data;
	pACK->result = result;
	App_Blue_SendData(pMsg,sizeof(BLUE_DOWN_FW_ACK_STR));
}


void App_BlueFWDownLoad(BLUE_PROTO_STR *pMsg)
{
	uint8_t result = 0;
	BLUE_FW_DOWNLOAD_REQ_STR *pFW = (void*)pMsg->data;	
	uint8_t index = pFW->data[0];

	//保存数据 todo

	//ACK
	BLUE_FW_DOWNLOAD_ACK_STR *pACK = (void*)pMsg->data;
	pACK->result = result;
	pACK->index = index;
	App_Blue_SendData(pMsg,sizeof(BLUE_FW_DOWNLOAD_ACK_STR));
}


void App_BlueSetWifiInfoProc(BLUE_PROTO_STR *pMsg)
{
	BLUE_SET_WIFI_INFO_STR *pWifi = (void*)pMsg->data;
	CL_LOG("set wifi info:ssid=%s passws=%s \r\n",pWifi->ssid,pWifi->passwd);
	memset(SystemInfo.WifiName,0,sizeof(SystemInfo.WifiName));
	memset(SystemInfo.WifiPasswd,0,sizeof(SystemInfo.WifiPasswd));
	sprintf(SystemInfo.WifiName,"\"%s\"",pWifi->ssid);
	sprintf(SystemInfo.WifiPasswd,"\"%s\"",pWifi->passwd);
	FlashWriteSysInfo(&SystemInfo,sizeof(SystemInfo));

	//ACK
	BLUE_SET_WIFI_INFO_ACK *pAck = (void*)pMsg->data;
	pAck->result = 0;
	App_Blue_SendData(pMsg,sizeof(BLUE_SET_WIFI_INFO_ACK));

	Sc8042bSpeech(VOIC_SUCCESS);
	osDelay(1000);
	BswSrv_SystemReboot();
}


void App_Blue_SendHeartBat(void)
{
	uint8_t pbuffer[64]={0};

	BLUE_PROTO_STR *pMsg = (void*)pbuffer;
	HEART_BEAT_STR *heartbat = (void*)pMsg->data;

	heartbat->simSignal = GlobalInfo.simSignal;
	heartbat->temp = BswSrv_GetCpuTemp() + 50;
	heartbat->portCnt = 0;
	App_Blue_SendData(pMsg,sizeof(HEART_BEAT_STR));
}

void App_HandleBlueFrame(uint8_t *data,uint16_t len)
{
    BLUE_PROTO_STR *pMsg = (void*)(data);
	
	if (CL_OK != CheckBluePkt(pMsg, len)) {
		CL_LOG("CheckBluePkt check error.\n");
		return;
	}

	switch (pMsg->head.cmd) {
        case B_SHAKE_REQ:	//握手请求
            CL_LOG("BlueProtoProc shake req.\n");
            App_BlueShakeReqProc(pMsg);
            break;
		case B_HEART_BEAT://心跳响应
			CL_LOG("BlueProtoProc heart beat ack.\n");
			App_BlueSendHeartBeatACK(pMsg);
			break;
        case B_REQ_BREAK://请求断开蓝牙链接
            CL_LOG("BlueProtoProc req break.\n");
            App_BlueReqBreak(pMsg);
            break;
		case B_SET_DEV_SERIALNUM://设置充电桩编号
			CL_LOG("BlueProtoProc set chargersn.\n");
			App_BlueSetChargerSn(pMsg);
			break;
		case B_SET_DEV_CODER://设置充电桩识别码
			CL_LOG("BlueProtoProc set dev code.\n");
			App_BlueSetChargerICode(pMsg);
			break;
		case B_REMOTE_CTRL://远程控制
			CL_LOG("BlueProtoProc remote ctrl.\n");
			App_BlueRemoteCtrl(pMsg);
			break;
		case B_FW_UPGRADE_NOTICE://固件升级请求
			App_BlueFWUpgradeNotice(pMsg);
			break;
		case B_FW_DOWN_LOAD://固件下发
			App_BlueFWDownLoad(pMsg);
			break;
		case B_SET_WIFI_INFO://设置WiFi信息
			CL_LOG("BlueProtoProc set wifi infi .\n");
			App_BlueSetWifiInfoProc(pMsg);
			break;
		default:
			CL_LOG("blue no find cmd");
			break;
    }
}

