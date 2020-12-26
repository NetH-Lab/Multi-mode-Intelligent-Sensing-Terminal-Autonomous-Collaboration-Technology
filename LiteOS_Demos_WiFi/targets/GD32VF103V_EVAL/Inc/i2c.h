/*
 * i2c.h
 *
 *  Created on: 2020年12月10日
 *      Author: LabWU
 */

#ifndef DEMOS_HARDWARE_DEMO_I2C_I2C_H_
#define DEMOS_HARDWARE_DEMO_I2C_I2C_H_

#include "gd32vf103.h"
#include "systick.h"
#include "stdio.h"

#define ADDR  0x23//0100011
#define uchar unsigned char
typedef unsigned short int  u16;
typedef unsigned   char uint8_t;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;


#define BHAddWrite     0x46      //从机地址+最后写方向位
#define BHAddRead      0x47      //从机地址+最后读方向位
#define BHPowDown      0x00      //关闭模块
#define BHPowOn        0x01      //打开模块等待测量指令
#define BHReset        0x07      //重置数据寄存器值在PowerOn模式下有效
#define BHModeH1       0x10      //高分辨率 单位1lx 测量时间120ms
#define BHModeH2       0x11      //高分辨率模式2 单位0.5lx 测量时间120ms
#define BHModeL        0x13      //低分辨率 单位4lx 测量时间16ms
#define BHSigModeH     0x20      //一次高分辨率 测量 测量后模块转到 PowerDown模式
#define BHSigModeH2    0x21      //同上类似
#define BHSigModeL     0x23      // 上类似

void IIC_Init(void);
void IIC_ACK(void);
void IIC_NACK(void);
u8 IIC_Wait_Ack(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(u8 byte);
u8 IIC_Read_Byte(unsigned char ack);
u8 IIC_RcvByte(void);
u16 bh_data_read(void);
void bh_data_send(u8 command);

#endif /* DEMOS_HARDWARE_DEMO_I2C_I2C_H_ */



