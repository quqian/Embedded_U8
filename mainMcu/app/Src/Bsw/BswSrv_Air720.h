#ifndef __AIRM2M_H__
#define __AIRM2M_H__

#include "includes.h"

//#define AIR720_DEBUG

#define SET_ATS0        1	//是否设置自动接听电话

typedef enum{
    GSM_RESET = 0,      //0.模块复位
    GSM_TEST,           //模块测试
	GSM_ATE,			//设置回显
	GSM_ATI,			//显示产品ID信息
	GSM_CPIN,
	GSM_CCID,		    //查看SIM卡的CCID
	GSM_CSQ,			//查看信号质量
    #if(SET_ATS0 == 1)
	GSM_ATS0,		    //自动接听
    #endif
	GSM_CIPQSEND,	    //设置非透传数据发送模式
	GSM_CIPRXGET,		//自动接收
	GSM_CIPMUX,		    //设置TCP单连接 
	GSM_CIPSHUT,		//关闭移动场景
	GSM_CREG,
	GSM_DIALG,
	GSM_IPOPEN,		    //创建TCP连接
	GSM_STATE_NUM,
}GSM_STEP;

typedef enum{
	IP_INITIAL = 0,
	IP_START,
	IP_CONFIG,
	IP_GPRSACT,
	IP_STATUS,
	TCP_CONNECTING,
	CONNECT_OK,
	TCP_CLOSING,
	TCP_CLOSED,
	PDP_DEACT,
	SOCKET_ERROR 
}SOCKET_STATE;

typedef struct{
    char *cmd;
    char *res;
    int wait;                               //at指令等待应答时间
    int nwait;                              //重试间隔时间
    int (*process)(char ok, uint8_t retry);
}gsm_inittab;

int BswSrv_Air720_GetSocketState(int retry);
int BswSrv_Air720_GetSignal(int retry);
int BswSrv_Air720_Iccid(int retry);
void BswSrv_Air720_CloseSocket(void);
int BswSrv_Air720_SendData(int socket,uint8_t* data, uint16_t len);

int BswSrv_Air720_Reconnect(GSM_STEP step);

int BswSrv_Air720_FtpGet(const char* serv, const char* un, const char* pw, const char* file);

int BswSrv_Air720_StartUp(void);
void BswSrv_Air720_Init(void);


#endif
