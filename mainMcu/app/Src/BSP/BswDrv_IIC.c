/*
 * @Author: zhoumin 
 * @Date: 2018-10-29 18:05:36 
 * @Last Modified by:   zhoumin 
 * @Last Modified time: 2018-10-29 18:05:36 
 */

#include "includes.h"
#include "BswDrv_IIC.h"
#include "BswDrv_Delay.h"

#define DELAY_US	20

static void SimIIC_Init(void);

static int IIC_ReadData(IIC_Typedef iic,uint8_t addr,uint8_t *data,uint16_t len);
static int IIC_SendData(IIC_Typedef iic,uint8_t addr,uint8_t *data,uint16_t len);

static int SimIIC_ReadData(uint8_t addr,uint8_t *data,uint16_t len);
static int SimIIC_SendData(uint8_t addr,uint8_t *data,uint16_t len);

void IIC0_Init(uint8_t address)
{
     /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);

    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);

    /* I2C clock configure */
    i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
    /* I2C address configure */
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, address);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}


void SimIIC_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6);  //开漏输出
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);  //推挽输出

    IIC_SCL_SET;
    IIC_SDA_SET;

}


int IIC_SendData(IIC_Typedef iic,uint8_t addr,uint8_t *data,uint16_t len)
{
    while(i2c_flag_get(iic, I2C_FLAG_I2CBSY));

    //发送启动条件
    i2c_start_on_bus(iic);
    while(!i2c_flag_get(iic, I2C_FLAG_SBSEND));

    //发送地址--写地址
    i2c_master_addressing(iic, addr, I2C_TRANSMITTER);
    while(!i2c_flag_get(iic, I2C_FLAG_ADDSEND));
    i2c_flag_clear(iic, I2C_FLAG_ADDSEND);

    while(!i2c_flag_get(iic, I2C_FLAG_TBE));
    for(uint16_t i = 0;i<len;i++)
    {
        i2c_data_transmit(iic,data[i]);
        while(!i2c_flag_get(iic, I2C_FLAG_TBE));
    }
    //发送结束条件
    i2c_stop_on_bus(iic);
    while(I2C_CTL0(iic)&0x0200);

    return CL_OK;
}


int IIC_ReadData(IIC_Typedef iic,uint8_t addr,uint8_t *data,uint16_t len)
{
    if(0 == len) 
    {
        return CL_FAIL;
    }
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
	
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, addr, I2C_RECEIVER);
    /* wait until ADDSEND bit is set */
   while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

    for(uint16_t i = 0;i<len;i++)
    {
        /* wait until the RBNE bit is set */
         while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
        /* read a data from I2C_DATA */
        data[i] = i2c_data_receive(I2C0);
    }
	
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    while(I2C_CTL0(I2C0)&0x0200);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);

    return CL_OK;
}


//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT(); //输出

	IIC_SCL_SET;
    IIC_SDA_SET;
    BswDrv_SoftDelay_us(DELAY_US);
    while(READ_SCL != 1){ };    //确认SCL 已经置高

    IIC_SDA_CLR;
	BswDrv_SoftDelay_us(DELAY_US);

	IIC_SCL_CLR;
    BswDrv_SoftDelay_us(DELAY_US);
}	

//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();   //输出

    IIC_SDA_CLR;
    BswDrv_SoftDelay_us(DELAY_US);

    IIC_SCL_SET;
    BswDrv_SoftDelay_us(DELAY_US);
    while(READ_SCL != 1){ }; //确认SCL 已经置高

    IIC_SDA_SET;
	BswDrv_SoftDelay_us(DELAY_US);							   	
}


//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
int IIC_Wait_Ack(void)
{
    SDA_IN();      //SDA设置为输入 

    IIC_SCL_CLR;
    BswDrv_SoftDelay_us(DELAY_US);

    IIC_SCL_SET;
    BswDrv_SoftDelay_us(DELAY_US);

    if(READ_SDA)
    {
        IIC_Stop();
        return CL_FAIL;
    }

    IIC_SCL_CLR;
    BswDrv_SoftDelay_us(DELAY_US);

    return CL_OK;
} 


//产生ACK应答 ack:1-产生应答 0-非应答
void IIC_Ack(uint8_t ack)
{
    SDA_OUT();

	IIC_SCL_CLR;
    BswDrv_SoftDelay_us(DELAY_US);

    if(ack == 1)
    {
        IIC_SDA_CLR;
    }else
    {
        IIC_SDA_SET;
    }
    BswDrv_SoftDelay_us(DELAY_US);

    IIC_SCL_SET;
    BswDrv_SoftDelay_us(DELAY_US);
    IIC_SCL_CLR;
    BswDrv_SoftDelay_us(DELAY_US);
}


//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
int IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    //输出

    for(t=0;t<8;t++)
    {
        IIC_SCL_CLR;
        BswDrv_SoftDelay_us(DELAY_US);

		if((txd & 0x80))
			IIC_SDA_SET;
		else
			IIC_SDA_CLR;

		txd<<=1;
		BswDrv_SoftDelay_us(DELAY_US); 

		IIC_SCL_SET;
		BswDrv_SoftDelay_us(DELAY_US); 
        while(READ_SCL != 1){ };    // 确认SCL 已经置高
    }

    IIC_SCL_CLR;
    BswDrv_SoftDelay_us(DELAY_US);

    return IIC_Wait_Ack();
} 

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(uint8_t ack)
{
	uint8_t receive = 0;
	SDA_IN();//SDA设置为输入

    for(uint8_t i=0;i<8;i++)
	{
        IIC_SCL_CLR;
		BswDrv_SoftDelay_us(DELAY_US); 

        IIC_SCL_SET; 
        BswDrv_SoftDelay_us(DELAY_US);
        while(READ_SCL != 1){ };    //确认SCL 已经置高

        receive<<=1;
        if(READ_SDA)
        {
            receive |= 0x01;
        }
    }		

    IIC_Ack(ack);//发送ack

    return receive;
}


int SimIIC_SendData(uint8_t addr,uint8_t *data,uint16_t len)
{
    int ack = 1;
    IIC_Start();    //启动条件

    ack = IIC_Send_Byte(addr);   //发送地址
    if(ack!= CL_OK)
    {
        CL_LOG("IIC_Send_Byte failed..\r\n");
        return CL_FAIL;
    }
    for(uint16_t i = 0;i<len;i++)
    {
        ack = IIC_Send_Byte(data[i]);
        if(ack!= CL_OK)
        {
            CL_LOG("IIC_Send_Byte failed..\r\n");
            return CL_FAIL;
        }
    }
    IIC_Stop();

    return CL_OK;
}


int SimIIC_ReadData(uint8_t addr,uint8_t *data,uint16_t len)
{
    int ack = 1;
    IIC_Start();    //启动条件

    ack = IIC_Send_Byte(addr);   //发送地址
    if(ack!= CL_OK)
    {
        CL_LOG("IIC_Send_Byte failed..\r\n");
        return CL_FAIL;
    }    

    for(uint16_t i = 0;i<len;i++)
    {
        if(i < len-1)
        {
            data[i] = IIC_Read_Byte(1);
        }else{
            data[i] = IIC_Read_Byte(0);
        }
    }
    IIC_Stop();

    return CL_OK;
}


int BswDrv_IIC_ReadData(IIC_Typedef iic,uint8_t addr,uint8_t *data,uint16_t len)
{
    if(len == 0)
    {
        return CL_FAIL;
    }

    if(iic == SIM_IIC)
    {
        return SimIIC_ReadData(addr,data,len);
    }else{
        return IIC_ReadData(iic,addr,data,len);
    }
}


int BswDrv_IIC_WritedData(IIC_Typedef iic,uint8_t addr,uint8_t *data,uint16_t len)
{
    if(len == 0)
    {
        return CL_FAIL;
    }

    if(iic == SIM_IIC)
    {
        return SimIIC_SendData(addr,data,len);
    }else{
        return IIC_SendData(iic,addr,data,len);
    }
}

void BswDrv_IIC_init(void)
{
    SimIIC_Init();
}
