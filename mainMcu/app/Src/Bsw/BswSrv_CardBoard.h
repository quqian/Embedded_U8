#ifndef __BSWSRV_CARDBOARD_H__
#define __BSWSRV_CARDBOARD_H__


#include <stdint.h>

#define CB_PKT_LEN  128

#define CB_POWER_ON()	gpio_bit_set(GPIOC,GPIO_PIN_2);
#define CB_POWER_OFF()	gpio_bit_reset(GPIOC,GPIO_PIN_2);


typedef enum {
	CB_FIND_AA=0,
	CB_FIND_FIVE=1,
	CB_FIND_LEN=2,
	CB_FIND_VER=3,
	CB_FIND_SN=4,
	CB_FIND_MODULE=5,
	CB_FIND_CMD=6,
	CB_FIND_DATA=7,
	CB_FIND_CHECKSUM=8,
}CB_FIND;


void App_CB_DownFW(uint8_t package,uint8_t *data,uint16_t len);
void App_CB_SendStartUpgrade(uint32_t fileSize,uint32_t package,uint16_t checkSum,uint8_t verson);

void BswSrv_CB_RecvData(void);
void BswSrv_CB_SendData(uint8_t *data,uint16_t len);

void BswSrv_CB_Reset(void);
void BswSrv_CB_Init(void);


#endif
