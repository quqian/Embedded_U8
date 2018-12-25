
#ifndef SYS_TICK_H
#define SYS_TICK_H

#include <stdint.h>

/* configure systick */
extern void systick_config(void);
/* delay a time in milliseconds */
extern void delay_1ms(uint32_t count);
/* delay decrement */
extern void delay_decrement(void);



extern void SystickInit(void);
extern void DelayMsWithNoneOs(uint32_t nms);
extern void DelayUs(uint32_t us);
extern void Delay100Us(void);

#endif /* SYS_TICK_H */




