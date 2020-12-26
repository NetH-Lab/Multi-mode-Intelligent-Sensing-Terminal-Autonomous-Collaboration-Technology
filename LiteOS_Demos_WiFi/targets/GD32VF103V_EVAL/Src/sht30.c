/*
 * sht30.c
 *
 *  Created on: 2020年12月10日
 *      Author: LabWU
 */

#ifndef TARGETS_GD32VF103V_EVAL_SRC_SHT30_C_
#define TARGETS_GD32VF103V_EVAL_SRC_SHT30_C_

#include "i2c.h"
#include "sht30.h"
#include "systick.h"
#include <los_task.h>

float_data humidity;    //存储的湿度数据
float_data temperature; //存储的温度数据
uint8_t status[3];           //状态寄存器
uint8_t status_crc;          // CRC校验计算
uint8_t crc_check;           // crc结果

//如果读取不到数据，有可能是读取的时间设得太短导致，我这里的时间是最小可实现读取的时间！！！

// CRC校验
static uint8_t CRC_Check(uint8_t *check_data, uint8_t num, uint8_t check_crc)
{
    uint8_t bit;        // bit mask
    uint8_t crc = 0xFF; // calculated checksum
    uint8_t byteCtr;    // byte counter

    // calculates 8-Bit checksum with given polynomial x8+x5+x4+1
    for (byteCtr = 0; byteCtr < num; byteCtr++)
    {
        crc ^= (*(check_data + byteCtr));
        // crc校验，最高位是1就^0x31
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

//根据所选获取数据的方式初始化
void SHT30_Init(void)
{
	/* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
	/* connect PB6 to I2C0_SCL */
	/* connect PB7 to I2C0_SDA */
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
    SHT30_General_RESET();  //软复位设备
    delay_1ms(1000);
}

//发送两字节指令，stop确定是否发送停止信号
void SHT30_SendBytes(uint16_t cmd, uint8_t stop)
{
    IIC_Start();
    IIC_Send_Byte(SHT30_ADDR << 1 | I2C_WR); //写7位I2C设备地址加0作为写取位,1为读取位
    IIC_Wait_Ack();
    IIC_Send_Byte(cmd >> 8);
    IIC_Wait_Ack();
    IIC_Send_Byte(cmd & 0xFF);
    IIC_Wait_Ack();
    if (stop)
        IIC_Stop();
}

//软复位
void SHT30_SOFTRESET(void) { SHT30_SendBytes(0x30A2, 1); }

//通用复位
void SHT30_General_RESET(void)
{
    IIC_Start();
    IIC_Send_Byte(0x00);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x06);
    IIC_Wait_Ack();
}

//获取状态寄存器数据
void SHT30_Get_Status(uint8_t *buffer)
{
    SHT30_SendBytes(0xF32D, 0);
    delay_1ms(3);
    IIC_Start();
    IIC_Send_Byte(SHT30_ADDR << 1 | I2C_RD); //写7位I2C设备地址加0作为写取位,1为读取位
    if (IIC_Wait_Ack() == 0)
    {
        buffer[0] = IIC_Read_Byte(1);
        buffer[1] = IIC_Read_Byte(1);
        buffer[2] = IIC_Read_Byte(1);
        IIC_Stop();
    }
}


//清空状态寄存器
void SHT30_Clear_Status(void) { SHT30_SendBytes(0x3041, 1); }

//获取SHT3x温湿度
void SHT30_Read(float* Temperature, float* Humidity)
{
	uint8_t buff[6];
	uint16_t tem, hum;
	uint8_t crcT, crcH;
    SHT30_Single_Shot(buff);

    tem = ((buff[0] << 8) | buff[1]); //温度拼接
    hum = ((buff[3] << 8) | buff[4]); //湿度拼接

    //计算温度和湿度CRC校验码
    crcT = CRC_Check(buff, 2, buff[2]);     //温度
    crcH = CRC_Check(buff + 3, 2, buff[5]); //湿度

    if (crcT && crcH)
    {
        //根据手册计算公式计算
        *Temperature = (175.0 * (float)tem / 65535.0 - 45.0); // T = -45 + 175 * tem / (2^16-1)
        *Humidity = (100.0 * (float)hum / 65535.0);           // RH = hum*100 / (2^16-1)
    }
}

//返回0代表状态寄存器读取成功，1代表读取错误
uint8_t SHT30_Status(void)
{
    SHT30_Get_Status(status);

    if (CRC_Check(status, 2, *(status + 2)))
        return 0; //正确
    else
        return 1; //错误
}

//单次读取数据
void SHT30_Single_Shot(uint8_t *buffer)
{
	uint8_t try_time = 100;
    SHT30_SendBytes(0x2C06, 1);

    delay_1ms(1000); //很重要！不然读不出数据
    IIC_Start();
    IIC_Send_Byte(SHT30_ADDR << 1 | I2C_RD); //写7位I2C设备地址加0作为写取位,1为读取位

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

//开启/关闭加热器
void SHT30_Heater(uint8_t enable)
{
    if (enable)
        SHT30_SendBytes(0x306D, 1);
    else
        SHT30_SendBytes(0x3066, 1);
}

//加速响应指令
void SHT30_ART(void) { SHT30_SendBytes(0x2B32, 0); }

//配置周期读取重复性和采样速率
void SHT30_Periodic_SetRepeat(void)
{
    SHT30_SendBytes(0x2737, 0); //高重复度，mps为10
}

//配置周期读取总配置
void SHT30_Periodic_Config(void)
{
    //配置
    SHT30_Periodic_SetRepeat();
    SHT30_ART();
}
//周期读取数据 如果使用就要在初始化时加上SHT30_Periodic_Config()函数
void SHT30_Periodic(uint8_t *buffer)
{
	uint8_t try_time = 100;

    //获取数据
    SHT30_SendBytes(0xE000, 0);
    delay_1ms(3); //很重要！不然读不出数据

    IIC_Start();
    IIC_Send_Byte(SHT30_ADDR << 1 | I2C_RD); //写7位I2C设备地址加0作为写取位,1为读取位

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

//中断指令/停止周期获取数据功能
void SHT30_Stop_Break(void)
{
    SHT30_SendBytes(0x3093, 1);
    delay_1ms(15);
}

//采集sht30 温度以及湿度数据
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
