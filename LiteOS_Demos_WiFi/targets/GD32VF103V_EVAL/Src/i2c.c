/*
 * i2c.c
 *
 *  Created on: 2020��12��10��
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


//����IIC��ʼ�ź�
void IIC_Start(void)
{
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA����Ϊ���//sda�����
	gpio_bit_set(GPIOB,GPIO_PIN_6);
	gpio_bit_set(GPIOB,GPIO_PIN_7);
	delay_1us(4);
	gpio_bit_reset(GPIOB,GPIO_PIN_7);//START:when CLK is high,DATA change form high to low
	delay_1us(4);
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//ǯסI2C���ߣ�׼�����ͻ��������
}

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA����Ϊ���//sda�����
	gpio_bit_reset(GPIOB,GPIO_PIN_6);
	gpio_bit_reset(GPIOB,GPIO_PIN_7);//STOP:when CLK is high DATA change form low to high
	delay_1us(4);
 	gpio_bit_set(GPIOB,GPIO_PIN_6);
 	gpio_bit_set(GPIOB,GPIO_PIN_7);//����I2C���߽����ź�
 	delay_1us(4);
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA����Ϊ����
	gpio_bit_set(GPIOB,GPIO_PIN_7);
	delay_1us(1);
	gpio_bit_set(GPIOB,GPIO_PIN_6);
	delay_1us(1);
    // #define READ_SDA   PDin(7)  //����SDA
	while(gpio_input_bit_get(GPIOB,GPIO_PIN_7))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;//ʱ�����0
	return 0;
}
//����ACKӦ��
void IIC_Ack(void)
{
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA����Ϊ���
	gpio_bit_reset(GPIOB,GPIO_PIN_7);//IIC_SDA=0;
	delay_1us(2);
	gpio_bit_set(GPIOB,GPIO_PIN_6);//IIC_SCL=1;
	delay_1us(2);
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;
}
//������ACKӦ��
void IIC_NAck(void)
{
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA����Ϊ���
	gpio_bit_set(GPIOB,GPIO_PIN_7);//IIC_SDA=1;
	delay_1us(2);
	gpio_bit_set(GPIOB,GPIO_PIN_6);//IIC_SCL=1;
	delay_1us(2);
	gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA����Ϊ���//
    gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			gpio_bit_set(GPIOB,GPIO_PIN_7);//IIC_SDA=1;
		else
			gpio_bit_reset(GPIOB,GPIO_PIN_7);//IIC_SDA=0;
		txd<<=1;
		delay_1us(2);   //��TEA5767��������ʱ���Ǳ����
		gpio_bit_set(GPIOB,GPIO_PIN_6);//IIC_SCL=1;
		delay_1us(2);
		gpio_bit_reset(GPIOB,GPIO_PIN_6);//IIC_SCL=0;
		delay_1us(2);
    }
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7); //SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK
    return receive;
}

void bh_data_send(u8 command)
{
    do{
    IIC_Start();                      //iic��ʼ�ź�
    IIC_Send_Byte(BHAddWrite);        //����������ַ
    }while(IIC_Wait_Ack());           //�ȴ��ӻ�Ӧ��
    IIC_Send_Byte(command);           //����ָ��
    IIC_Wait_Ack();                   //�ȴ��ӻ�Ӧ��
    IIC_Stop();                       //iicֹͣ�ź�
}

u16 bh_data_read(void)
{
	u16 buf;
	IIC_Start();                       //iic��ʼ�ź�
	IIC_Send_Byte(BHAddRead);         //����������ַ+����־λ
	IIC_Wait_Ack();                     //�ȴ��ӻ�Ӧ��
	buf=IIC_Read_Byte(1);              //��ȡ����
	buf=buf<<8;                        //��ȡ������߰�λ����
	buf+=0x00ff&IIC_Read_Byte(0);      //��ȡ������ڰ�λ����
	IIC_Stop();                        //����ֹͣ�ź�
	return buf;
}



