#ifndef _FUSION_H_
#define _FUSION_H_

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "route.h"

/*-------------------------definition-------------------------*/
#define fire_sensor_num 3
#define GET_ARRAY_LEN(array) sizeof(array) / sizeof(array[0]) //计算数组长度

/*-------------------------return value-------------------------*/
#define RETVAL_FUSION_FAILED 0              //return value: failed to 
#define RETVAL_FUSION_SUCCESSFULLY 1        //return value: successfully

typedef unsigned char uint8_t;

float Membership(float data, float x1, float x2, float x3, float y1, float y2);
float data_fusion(float Fire_data[]);
double mean(double data[], int length);
int cooperative_control(float fusion_result);
#endif 