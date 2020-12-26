/*
 * sht30.c
 *
 *  Created on: 2020��12��10��
 *      Author: LabWU
 */

#ifndef TARGETS_GD32VF103V_EVAL_SRC_SHT30_C_
#define TARGETS_GD32VF103V_EVAL_SRC_SHT30_C_

#include "i2c.h"
#include "sht30.h"
#include "systick.h"
#include <los_task.h>

float_data humidity;    //�洢��ʪ������
float_data temperature; //�洢���¶�����
uint8_t status[3];           //״̬�Ĵ���
uint8_t status_crc;          // CRCУ�����
uint8_t crc_check;           // crc���

//�����ȡ�������ݣ��п����Ƕ�ȡ��ʱ�����̫�̵��£��������ʱ������С��ʵ�ֶ�ȡ��ʱ�䣡����

// CRCУ��
static uint8_t CRC_Check(uint8_t *check_data, uint8_t num, uint8_t check_crc)
{
    uint8_t bit;        // bit mask
    uint8_t crc = 0xFF; // calculated checksum
    uint8_t byteCtr;    // byte counter

    // calculates 8-Bit checksum with given polynomial x8+x5+x4+1
    for (byteCtr = 0; byteCtr < num; byteCtr++)
    {
        crc ^= (*(check_data + byteCtr));
        // crcУ�飬���λ��1��^0x31
        for (bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }
    if (crc == check_crc)
        return 1;
    else
        return 0;
}

//������ѡ��ȡ���ݵķ�ʽ��ʼ��
void SHT30_Init(void)
{
	/* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
	/* connect PB6 to I2C0_SCL */
	/* connect PB7 to I2C0_SDA */
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
    SHT30_General_RESET();  //��λ�豸
    delay_1ms(1000);
}

//�������ֽ�ָ�stopȷ���Ƿ���ֹͣ�ź�
void SHT30_SendBytes(uint16_t cmd, uint8_t stop)
{
    IIC_Start();
    IIC_Send_Byte(SHT30_ADDR << 1 | I2C_WR); //д7λI2C�豸��ַ��0��Ϊдȡλ,1Ϊ��ȡλ
    IIC_Wait_Ack();
    IIC_Send_Byte(cmd >> 8);
    IIC_Wait_Ack();
    IIC_Send_Byte(cmd & 0xFF);
    IIC_Wait_Ack();
    if (stop)
        IIC_Stop();
}

//��λ
void SHT30_SOFTRESET(void) { SHT30_SendBytes(0x30A2, 1); }

//ͨ�ø�λ
void SHT30_General_RESET(void)
{
    IIC_Start();
    IIC_Send_Byte(0x00);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x06);
    IIC_Wait_Ack();
}

//��ȡ״̬�Ĵ�������
void SHT30_Get_Status(uint8_t *buffer)
{
    SHT30_SendBytes(0xF32D, 0);
    delay_1ms(3);
    IIC_Start();
    IIC_Send_Byte(SHT30_ADDR << 1 | I2C_RD); //д7λI2C�豸��ַ��0��Ϊдȡλ,1Ϊ��ȡλ
    if (IIC_Wait_Ack() == 0)
    {
        buffer[0] = IIC_Read_Byte(1);
        buffer[1] = IIC_Read_Byte(1);
        buffer[2] = IIC_Read_Byte(1);
        IIC_Stop();
    }
}


//���״̬�Ĵ���
void SHT30_Clear_Status(void) { SHT30_SendBytes(0x3041, 1); }

//��ȡSHT3x��ʪ��
void SHT30_Read(float* Temperature, float* Humidity)
{
	uint8_t buff[6];
	uint16_t tem, hum;
	uint8_t crcT, crcH;
    SHT30_Single_Shot(buff);

    tem = ((buff[0] << 8) | buff[1]); //�¶�ƴ��
    hum = ((buff[3] << 8) | buff[4]); //ʪ��ƴ��

    //�����¶Ⱥ�ʪ��CRCУ����
    crcT = CRC_Check(buff, 2, buff[2]);     //�¶�
    crcH = CRC_Check(buff + 3, 2, buff[5]); //ʪ��

    if (crcT && crcH)
    {
        //�����ֲ���㹫ʽ����
        *Temperature = (175.0 * (float)tem / 65535.0 - 45.0); // T = -45 + 175 * tem / (2^16-1)
        *Humidity = (100.0 * (float)hum / 65535.0);           // RH = hum*100 / (2^16-1)
    }
}

//����0����״̬�Ĵ�����ȡ�ɹ���1�����ȡ����
uint8_t SHT30_Status(void)
{
    SHT30_Get_Status(status);

    if (CRC_Check(status, 2, *(status + 2)))
        return 0; //��ȷ
    else
        return 1; //����
}

//���ζ�ȡ����
void SHT30_Single_Shot(uint8_t *buffer)
{
	uint8_t try_time = 100;
    SHT30_SendBytes(0x2C06, 1);

    delay_1ms(1000); //����Ҫ����Ȼ����������
    IIC_Start();
    IIC_Send_Byte(SHT30_ADDR << 1 | I2C_RD); //д7λI2C�豸��ַ��0��Ϊдȡλ,1Ϊ��ȡλ

    while (IIC_Wait_Ack())
    {
        try_time--;
        delay_1us(50);
        if (try_time == 0)
            return;
    }
    buffer[0] = IIC_Read_Byte(1);
    buffer[1] = IIC_Read_Byte(1);
    buffer[2] = IIC_Read_Byte(1);
    buffer[3] = IIC_Read_Byte(1);
    buffer[4] = IIC_Read_Byte(1);
    buffer[5] = IIC_Read_Byte(0);
    IIC_Stop();
}

//����/�رռ�����
void SHT30_Heater(uint8_t enable)
{
    if (enable)
        SHT30_SendBytes(0x306D, 1);
    else
        SHT30_SendBytes(0x3066, 1);
}

//������Ӧָ��
void SHT30_ART(void) { SHT30_SendBytes(0x2B32, 0); }

//�������ڶ�ȡ�ظ��ԺͲ�������
void SHT30_Periodic_SetRepeat(void)
{
    SHT30_SendBytes(0x2737, 0); //���ظ��ȣ�mpsΪ10
}

//�������ڶ�ȡ������
void SHT30_Periodic_Config(void)
{
    //����
    SHT30_Periodic_SetRepeat();
    SHT30_ART();
}
//���ڶ�ȡ���� ���ʹ�þ�Ҫ�ڳ�ʼ��ʱ����SHT30_Periodic_Config()����
void SHT30_Periodic(uint8_t *buffer)
{
	uint8_t try_time = 100;

    //��ȡ����
    SHT30_SendBytes(0xE000, 0);
    delay_1ms(3); //����Ҫ����Ȼ����������

    IIC_Start();
    IIC_Send_Byte(SHT30_ADDR << 1 | I2C_RD); //д7λI2C�豸��ַ��0��Ϊдȡλ,1Ϊ��ȡλ

    while (IIC_Wait_Ack())
    {
        try_time--;
        delay_1us(50);
        if (try_time == 0)
            return;
    }
    buffer[0] = IIC_Read_Byte(1);
    buffer[1] = IIC_Read_Byte(1);
    buffer[2] = IIC_Read_Byte(1);
    buffer[3] = IIC_Read_Byte(1);
    buffer[4] = IIC_Read_Byte(1);
    buffer[5] = IIC_Read_Byte(0);
    IIC_Stop();
}

//�ж�ָ��/ֹͣ���ڻ�ȡ���ݹ���
void SHT30_Stop_Break(void)
{
    SHT30_SendBytes(0x3093, 1);
    delay_1ms(15);
}

//�ɼ�sht30 �¶��Լ�ʪ������
void Collect_Data_Sht30(float* Temperature, float* Humidity)
{
	 IIC_Init();
	 SHT30_Init();
	 float Tem_Hum[2];
	 SHT30_Read(&(Tem_Hum[0]), &(Tem_Hum[1]));
	 *Temperature = Tem_Hum[0];
	 *Humidity = Tem_Hum[1];
}

#endif /* TARGETS_GD32VF103V_EVAL_SRC_SHT30_C_ */
