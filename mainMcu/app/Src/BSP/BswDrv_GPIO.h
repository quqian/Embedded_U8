#ifndef __BSWDRV_GPIO_H__
#define __BSWDRV_GPIO_H__


#define LED_R_ON()   gpio_bit_set(GPIOA,GPIO_PIN_11)
#define LED_G_ON()   gpio_bit_set(GPIOA,GPIO_PIN_12)
#define LED_B_ON()   gpio_bit_reset(GPIOA,GPIO_PIN_8)
#define LED_G_TOGGLE()  gpio_bit_write(GPIOA,GPIO_PIN_12,(bit_status)(1-gpio_input_bit_get(GPIOA, GPIO_PIN_12)))

#define LED_R_OFF()   gpio_bit_reset(GPIOA,GPIO_PIN_11)
#define LED_G_OFF()   gpio_bit_reset(GPIOA,GPIO_PIN_12)
#define LED_B_OFF()   gpio_bit_set(GPIOA,GPIO_PIN_8)


////Ëø
//#define LOCK_HIGH()     gpio_bit_set(GPIOC,GPIO_PIN_14)
//#define LOCK_LOW()      gpio_bit_reset(GPIOC,GPIO_PIN_14)

//ÃÅ
#define DOOR_HIGH()     gpio_bit_set(GPIOC,GPIO_PIN_15)
#define DOOR_LOW()      gpio_bit_reset(GPIOC,GPIO_PIN_15)


#define READ_LVD_IN()   gpio_input_bit_get(GPIOC,GPIO_PIN_0)

#define READ_TP_IN()    gpio_input_bit_get(GPIOC,GPIO_PIN_4)

#define READ_DOOR_STATE()    gpio_input_bit_get(GPIOC,GPIO_PIN_1)

typedef enum{
    TYPE_SYNC = 1,
    TYPE_ASYNC = 2,
}DOOR_CTRL_TYPE;


void BswDrv_OpenDoor(DOOR_CTRL_TYPE flag);

void BswDrv_GPIO_Init(void);

#endif

