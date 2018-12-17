
#ifndef SYS_TICK_H
#define SYS_TICK_H

#include <stdint.h>

void BswDrv_HwDelay_100us(void);

void BswDrv_SoftDelay_ms(uint16_t nms);

void BswDrv_SoftDelay_us(uint16_t count);

#endif /* SYS_TICK_H */




