#ifndef __SPI_H__
#define __SPI_H__

#include "includes.h"


typedef enum{
	SPI_FM175x = SPI2,
}SPI_TYPE;


uint8_t BswDrv_SPI_ReadByte(SPI_TYPE type);
uint8_t BswDrv_SPI_ReadWriteByte(SPI_TYPE type,uint8_t value);

void BswDrv_SPI_Init(void);


#endif




