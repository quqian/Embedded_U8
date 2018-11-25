#include "spi.h"
#include "includes.h"
#include "systick.h"
#include "gd32f3x0_spi.h"


#define Bsp_Delay_us			DelayUs

/*****************************************************************************
** Function name:   	SpiInit
** Descriptions:        SPI初始化
** input parameters:    
** Returned value:	    None
** Author:              quqian
*****************************************************************************/
void SpiInit(void)
{
#if USER_SIMULATE_SPI
{
	rcu_periph_clock_enable(RCU_GPIOA);
	
	//NSS
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_4);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
	//CPND
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	//clock
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_5);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	//master out
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	//MISO
	gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
	
	//CPDN	高电平
	NPD_HIGH();
	//CS 高电平
	SPI_NSS_HIGH();
	//数据输出
	SPI_MOSI_HIGH();
	//时钟
	SPI_CLK_LOW();
}
#else
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_SPI0);
	rcu_periph_clock_enable(RCU_GPIOA);

    /* GPIOA config, PA5(SPI_CLK), PA6(SPI0_MISO), PA7(SPI_MOSI) */
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	
	//NSS
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_4);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
	//CPND
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	
    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_8;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    /* set crc polynomial */
    spi_crc_polynomial_set(SPI0, 7);
    spi_enable(SPI0);
#endif
}


/*****************************************************************************
** Function name:   	SpiReadWriteByte
** Descriptions:        send a byte through the SPI interface and return the byte received from the SPI bus
** input parameters:    byte: byte to send
** Returned value:	    the value of the received byte
** Author:              quqian
*****************************************************************************/
uint8_t SpiReadWriteByte(uint8_t value)
{
#if USER_SIMULATE_SPI
{
	uint8_t bit_ctr;
	
	for(bit_ctr = 0; bit_ctr < 8; bit_ctr++)   
	{
		if(value & 0x80)
		{
			SPI_MOSI_HIGH();
		}
		else
		{
			SPI_MOSI_LOW();		
		}

		SPI_CLK_HIGH();
		Bsp_Delay_us(1);
		value = (value << 1);   
		value |= SPI_MISO_READ();
		SPI_CLK_LOW();  
		Bsp_Delay_us(1);
	}
    
	return (value);      
}
#else
    /* loop while data register in not emplty */
    while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
    /* send byte through the SPI0 peripheral */
    spi_i2s_data_transmit(SPI0, value);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
    /* return the byte read from the SPI bus */
    return (spi_i2s_data_receive(SPI0));
#endif
}


/*****************************************************************************
** Function name:   	SpiReadByte
** Descriptions:        READ from the SPI bus
** input parameters:    
** Returned value:	    
** Author:              quqian
*****************************************************************************/
uint8_t SpiReadByte(void)
{
#if USER_SIMULATE_SPI
{
	#if 0
	uint8_t read_value = 0;
	uint8_t i = 0;
	
	for(i = 0; i < 8; i++)
	{
		SPI_CLK_HIGH();
		//OS_SPI_DELAY_MS(1);
        Bsp_Delay_us(1);
		read_value <<= 1;
		if(SPI_MISO_READ())
		{
		  read_value |= 0x01;
		}

		SPI_CLK_LOW();
 
		//OS_SPI_DELAY_MS(1);
        Bsp_Delay_us(1);
	}
 
	return(read_value);
	#else
	return SpiReadWriteByte(0xA5);
	#endif
}
#else
    return (SpiReadWriteByte(0xA5));
#endif
}



///*****************************************************************************
//** Function name:   	SpiSendHalfWord
//** Descriptions:        send a half word through the SPI interface and return the half word received from the SPI bus
//** input parameters:    half_word: half word to send
//** Returned value:	    the value of the received byte
//** Author:              quqian
//*****************************************************************************/
//uint16_t SpiSendHalfWord(uint16_t half_word)
//{
//    /* loop while data register in not emplty */
//    while(RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_TBE));

//    /* send half word through the SPI0 peripheral */
//    spi_i2s_data_transmit(SPI0,half_word);

//    /* wait to receive a half word */
//    while(RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_RBNE));

//    /* return the half word read from the SPI bus */
//    return spi_i2s_data_receive(SPI0);
//}

uint8_t SPITest(uint8_t send_data)
{
    uint8_t reg_value = 0;
    
	reg_value = SpiReadWriteByte(send_data);
    CL_LOG("reg_value1 = [%d]\r\n", reg_value);
    
	return(reg_value);
}
