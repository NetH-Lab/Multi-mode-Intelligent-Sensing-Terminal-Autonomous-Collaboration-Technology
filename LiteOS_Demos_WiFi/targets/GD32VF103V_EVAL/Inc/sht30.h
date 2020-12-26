/*
 * sht30.h
 *
 *  Created on: 2020年12月10日
 *      Author: LabWU
 */

#ifndef TARGETS_GD32VF103V_EVAL_INC_SHT30_H_
#define TARGETS_GD32VF103V_EVAL_INC_SHT30_H_

#include "i2c.h"
#include <stdint.h>
#define SHT30_ADDR      0x44
#define I2C_WR 0
#define I2C_RD 1

typedef union
{
    float fdata;
    unsigned char cdata[4];
}float_data;//定义联合体存储float数据，float类型的存储符合IEEE标准，可用于传输数据

void SHT30_Init(void);
void SHT30_SOFTRESET(void);
void SHT30_General_RESET(void);
void SHT30_Stop_Break(void);

void SHT30_Read(float* Temperature, float* Humidity);
void SHT30_SendBytes(uint16_t cmd,uint8_t stop);
void SHT30_ART(void);
void SHT30_Single_Shot(uint8_t *buffer);
void SHT30_Periodic(uint8_t *buffer);
void SHT30_Heater(uint8_t enable);

void Collect_Data_Sht30(float* Temperature, float* Humidity);



#endif /* TARGETS_GD32VF103V_EVAL_INC_SHT30_H_ */
