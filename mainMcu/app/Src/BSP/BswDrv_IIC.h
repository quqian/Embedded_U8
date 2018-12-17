#ifndef __BSWDRV_IIC_H__
#define __BSWDRV_IIC_H__



#include <stdint.h>

typedef enum{
    SIM_IIC = 1,
}IIC_Typedef;

#define SLAVE_ADDR      0x50
#define W_ADDR          0xA0
#define R_ADDR          0xA1

//DAT·½Ïò
#define SDA_IN()  gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ,  GPIO_PIN_7) 
#define SDA_OUT() gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_7) 


//SCL
#define IIC_SCL_SET     gpio_bit_set(GPIOB,GPIO_PIN_6)
#define IIC_SCL_CLR     gpio_bit_reset(GPIOB,GPIO_PIN_6)

//DAT
#define IIC_SDA_SET     gpio_bit_set(GPIOB,GPIO_PIN_7)
#define IIC_SDA_CLR     gpio_bit_reset(GPIOB,GPIO_PIN_7) 

#define READ_SDA        gpio_input_bit_get(GPIOB,GPIO_PIN_7)
#define READ_SCL        gpio_input_bit_get(GPIOB,GPIO_PIN_6)

void BswDrv_IIC_init(void);

int BswDrv_IIC_ReadData(IIC_Typedef iic,uint8_t addr,uint8_t *data,uint16_t len);
int BswDrv_IIC_WritedData(IIC_Typedef iic,uint8_t addr,uint8_t *data,uint16_t len);


#endif
