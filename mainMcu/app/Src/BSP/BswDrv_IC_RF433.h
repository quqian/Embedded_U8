#ifndef __RF433_H__
#define __RF433_H__



typedef enum 
{
	RF433_IDLE=0,
	RF433_SYNC,
	RF433_ADDR,
	RF433_DATA,
	RF433_STOP,
}HANDLE_RF433_DATA_STATE;


void BswDrv_RF433_Init(void);


#endif





