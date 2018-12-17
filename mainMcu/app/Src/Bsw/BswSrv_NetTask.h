#ifndef __NETTASK_H__
#define __NETTASK_H__



typedef enum {
    FIND_AA,
    FIND_55,
    FIND_CHARGER_TYPE,
    FIND_CHAGER_SN,
    FIND_SRC,
    FIND_DEST,
    FIND_LEN,
    FIND_VER,
    FIND_SERNUM,
    FIND_CMD,
    RX_DATA,
    FIND_CHK,
    FIND_END,
} PROTO_MSG_STR;

int BswSrv_SendSokcetData(uint8_t *data,uint16_t len);

void BswSrv_CloseNetWork(void);
int BswSrv_ResetNetWork(uint8_t type);
void SurfNet_Task(void);


#endif
