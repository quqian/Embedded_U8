#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "includes.h"




#define RED_LIGHT_ON()       			gpio_bit_set(GPIOB, GPIO_PIN_7)
#define RED_LIGHT_OFF()      			gpio_bit_reset(GPIOB, GPIO_PIN_7)

#define GREEN_LIGHT_ON()       			gpio_bit_set(GPIOB, GPIO_PIN_6)
#define GREEN_LIGHT_OFF()      			gpio_bit_reset(GPIOB, GPIO_PIN_6)

extern void LedInit(void);
extern void RedLed(void);
extern void GreenLed(void);


#endif




