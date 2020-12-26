/*
 * usart.h
 *
 *  Created on: 2020年12月6日
 *      Author: minel
 */

#ifndef _USART_H_
#define _USART_H_

/* Head Files */
#include <stdio.h>
#include <string.h>

#include "gd32vf103.h"
#include "gd32v103v_eval.h"
#include "gd32v103v_lcd_eval.h"
#include "touch_panel.h"
#include "picture.h"
#include "gd25qxx.h"
#include "systick.h"

#include "los_base.h"
#include "los_typedef.h"
#include "los_tick.h"

#include "mem.h"

#include "osal.h"

#include "los_swtmr.h"
#include "los_inspect_entry.h"
#include "los_api_task.h"
#include "task20_test.h"

#include "usart.h"
#include "liteos_all_tasks.h"
#include "wifitask.h"
#include <stdlib.h>
#include <timer.h>


/*传感器帧相关定义*/
#define sensor_data_length 4

/* Global Parameters */
#define RX_BUF_SIZE 200
extern uint8_t UART3_RX_BUF[RX_BUF_SIZE];
extern uint16_t UART3_RX_STA;
extern uint8_t UART4_RX_BUF[RX_BUF_SIZE];
extern uint16_t UART4_RX_STA;

extern UINT16 uart3_timer;


/* Functions */
uint8_t* get_usart_recv(uint16_t data_length, uint8_t* receivebuf);
void uart2_init(unsigned int bound);
void uart3_init(unsigned int bound);
void uart4_init(unsigned int bound);
void u3_printf(char* ch);
UINT32 UART2_Interrupt(VOID);
UINT32 UART3_Interrupt(VOID);
UINT32 UART4_Interrupt(VOID);

UINT32 UART3_swTimer(VOID);
VOID Timer_uart3_Callback(UINT32 arg);

extern uint8_t sensor_data[sensor_data_length];

#endif /* TARGETS_GD32VF103V_EVAL_INC_USART_H_ */
