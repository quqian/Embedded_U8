#ifndef __APP_UPGRADE_H__
#define __APP_UPGRADE_H__

#include "BswSrv_FwUpgrade.h"

void App_Upgrade_Init(UPGRADE_TYPE type,uint32_t package_num);

int App_Upgrade_Write(uint8_t *data,uint16_t datalen);

void App_Upgrade_Finish(uint8_t result);

#endif
