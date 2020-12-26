/*
 * i2c.c
 *
 *  Created on: 2020年12月10日
 *      Author: LabWU
 */

#include "i2c.h"
#include "gd32vf103.h"
#include "systick.h"

void IIC_Init()
{
	/* enable GPIOB clock */
	rcu_periph_clock_enable(RCU_GPIOB);
    /* connect PB6 to I2C0_SCL */
    /* connect PB7 to I2C0_SDA */
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
	gpio_bit_set(GPIOB,GPIO_PIN_6);
	gpio_bit_set(GPIOB,GPIO_PIN_7);
}


//产生IIC起始信号
void IIC_Start(void)
{
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA设置为输出//sda线输出
	gpio_bit_set(GPIOB,GPIO_PIN_6);
	gpio_bit_set(GPIOB,GPIO_PIN_7);
	delay_1us(4);
	gpio_bit_reset(GPIOB,GPIO_PIN_7);//START:when CLK is high,DATA change form high to low
	delay_1us(4);
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//钳住I2C总线，准备发送或接收数据
}

//产生IIC停止信号
void IIC_Stop(void)
{
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA设置为输出//sda线输出
	gpio_bit_reset(GPIOB,GPIO_PIN_6);
	gpio_bit_reset(GPIOB,GPIO_PIN_7);//STOP:when CLK is high DATA change form low to high
	delay_1us(4);
 	gpio_bit_set(GPIOB,GPIO_PIN_6);
 	gpio_bit_set(GPIOB,GPIO_PIN_7);//发送I2C总线结束信号
 	delay_1us(4);
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA设置为输入
	gpio_bit_set(GPIOB,GPIO_PIN_7);
	delay_1us(1);
	gpio_bit_set(GPIOB,GPIO_PIN_6);
	delay_1us(1);
    // #define READ_SDA   PDin(7)  //输入SDA
	while(gpio_input_bit_get(GPIOB,GPIO_PIN_7))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;//时钟输出0
	return 0;
}
//产生ACK应答
void IIC_Ack(void)
{
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA设置为输出
	gpio_bit_reset(GPIOB,GPIO_PIN_7);//IIC_SDA=0;
	delay_1us(2);
	gpio_bit_set(GPIOB,GPIO_PIN_6);//IIC_SCL=1;
	delay_1us(2);
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;
}
//不产生ACK应答
void IIC_NAck(void)
{
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA设置为输出
	gpio_bit_set(GPIOB,GPIO_PIN_7);//IIC_SDA=1;
	delay_1us(2);
	gpio_bit_set(GPIOB,GPIO_PIN_6);//IIC_SCL=1;
	delay_1us(2);
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA设置为输出//
    gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			gpio_bit_set(GPIOB,GPIO_PIN_7);//IIC_SDA=1;
		else
			gpio_bit_reset(GPIOB,GPIO_PIN_7);//IIC_SDA=0;
		txd<<=1;
		delay_1us(2);   //对TEA5767这三个延时都是必须的
		gpio_bit_set(GPIOB,GPIO_PIN_6);//IIC_SCL=1;
		delay_1us(2);
		gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;
		delay_1us(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA设置为输入
    for(i=0;i<8;i++ )
	{
    	gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;
    	delay_1us(2);
        gpio_bit_set(GPIOB,GPIO_PIN_6);//IIC_SCL=1;
        receive<<=1;
        if(gpio_input_bit_get(GPIOB,GPIO_PIN_7))
        receive++;
        delay_1us(1);
    }
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK
    return receive;
}

void bh_data_send(u8 command)
{
    do{
    IIC_Start();                      //iic起始信号
    IIC_Send_Byte(BHAddWrite);        //发送器件地址
    }while(IIC_Wait_Ack());           //等待从机应答
    IIC_Send_Byte(command);           //发送指令
    IIC_Wait_Ack();                   //等待从机应答
    IIC_Stop();                       //iic停止信号
}

u16 bh_data_read(void)
{
	u16 buf;
	IIC_Start();                       //iic起始信号
	IIC_Send_Byte(BHAddRead);         //发送器件地址+读标志位
	IIC_Wait_Ack();                     //等待从机应答
	buf=IIC_Read_Byte(1);              //读取数据
	buf=buf<<8;                        //读取并保存高八位数据
	buf+=0x00ff&IIC_Read_Byte(0);      //读取并保存第八位数据
	IIC_Stop();                        //发送停止信号
	return buf;
}



