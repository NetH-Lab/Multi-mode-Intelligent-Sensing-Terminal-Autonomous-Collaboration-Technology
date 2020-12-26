/*
 * i2c.h
 *
 *  Created on: 2020��12��10��
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


#define BHAddWrite     0x46      //�ӻ���ַ+���д����λ
#define BHAddRead      0x47      //�ӻ���ַ+��������λ
#define BHPowDown      0x00      //�ر�ģ��
#define BHPowOn        0x01      //��ģ��ȴ�����ָ��
#define BHReset        0x07      //�������ݼĴ���ֵ��PowerOnģʽ����Ч
#define BHModeH1       0x10      //�߷ֱ��� ��λ1lx ����ʱ��120ms
#define BHModeH2       0x11      //�߷ֱ���ģʽ2 ��λ0.5lx ����ʱ��120ms
#define BHModeL        0x13      //�ͷֱ��� ��λ4lx ����ʱ��16ms
#define BHSigModeH     0x20      //һ�θ߷ֱ��� ���� ������ģ��ת�� PowerDownģʽ
#define BHSigModeH2    0x21      //ͬ������
#define BHSigModeL     0x23      // ������

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



