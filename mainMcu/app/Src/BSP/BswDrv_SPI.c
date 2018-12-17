#include "includes.h"
#include "BswDrv_SPI.h"


static void BswDrv_SPI2_Init(void);


/*****************************************************************************
** Function name:   	SpiInit
** Descriptions:        SPI≥ı ºªØ
** input parameters:    
** Returned value:	    None
** Author:              quqian
*****************************************************************************/
void BswDrv_SPI2_Init(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI2);

    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_5);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);

    /* SPI parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_256 ;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI2, &spi_init_struct);

    /* set crc polynomial */
    // spi_crc_polynomial_set(SPI2,7);
    /* enable SPI2 */
    spi_enable(SPI2);
}


/*****************************************************************************
** Function name:   	SpiReadWriteByte
** Descriptions:        send a byte through the SPI interface and return the byte received from the SPI bus
** input parameters:    byte: byte to send
** Returned value:	    the value of the received byte
** Author:              quqian
*****************************************************************************/
uint8_t BswDrv_SPI_ReadWriteByte(SPI_TYPE type,uint8_t value)
{
    /* loop while data register in not emplty */
    while (RESET == spi_i2s_flag_get(type,SPI_FLAG_TBE));

    /* send byte through the SPI peripheral */
    spi_i2s_data_transmit(type, value);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(type,SPI_FLAG_RBNE));

    /* return the byte read from the SPI bus */
    return(spi_i2s_data_receive(type));

}


/*****************************************************************************
** Function name:   	SpiReadByte
** Descriptions:        READ from the SPI bus
** input parameters:    
** Returned value:	    
** Author:              quqian
*****************************************************************************/
uint8_t BswDrv_SPI_ReadByte(SPI_TYPE type)
{
    return (BswDrv_SPI_ReadWriteByte(type,0x00));
}




void BswDrv_SPI_Init(void)
{
	BswDrv_SPI2_Init();
}
