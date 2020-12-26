/*
 * timer.h
 *
 *  Created on: 2020Äê12ÔÂ9ÈÕ
 *      Author: Administrator
 */

#ifndef DEMOS_HARDWARE_DEMO_TIMER_TIMER_H_
#define DEMOS_HARDWARE_DEMO_TIMER_TIMER_H_
#include <stdio.h>
#include "los_swtmr.h"
#include "los_sys.h"
#include "los_api_timer.h"
#include "gd32vf103.h"

typedef unsigned   char uint8_t;
typedef unsigned short int  uint16_t;
extern UINT16 timer_id;// timer id
extern UINT32 sensor_function_start_co;
extern UINT32 sensor_function_start_smoke;
extern UINT32 sensor_function_start_temp;


extern UINT32 Led_Sensor_swTimer(uint16_t fre );


#endif /* DEMOS_HARDWARE_DEMO_TIMER_TIMER_H_ */
