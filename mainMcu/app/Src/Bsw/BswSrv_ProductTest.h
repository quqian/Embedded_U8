#ifndef __BSWSRV_CONFIGTOOL_H__
#define __BSWSRV_CONFIGTOOL_H__

#include <stdint.h>

#define CT_FRAME_LEN        200

enum {
    CT_FIND_AA=0,
    CT_FIND_55,
    CT_FIND_CHARGER_TYPE,
    CT_FIND_CHAGER_SN,
    CT_FIND_SRC,
    CT_FIND_DEST,
    CT_FIND_LEN,
    CT_FIND_VER,
    CT_FIND_SERNUM,
    CT_FIND_CMD,
    CT_RX_DATA,
    CT_FIND_CHK,
    CT_FIND_END,
};

int BswSrv_ProTest_SendData(uint8_t *data,uint16_t len);

void BswSrv_ProTest_RecvData(void);


#endif
