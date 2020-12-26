/*
 * liteos_all_tasks.h
 *
 *  Created on: 2020年12月6日
 *      Author: minel
 */

#ifndef LITEOS_ALL_TASKS_H_
#define LITEOS_ALL_TASKS_H_


#include "../../../demos/kernel_demo/include/los_inspect_entry.h"
#include "../../../drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Include/gd32vf103_gpio.h"
#include "../../../drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Include/gd32vf103_rcu.h"
#include "../../../drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Include/gd32vf103_usart.h"
#include "../../../iot_link/os/os_imp/liteos/base/include/mem.h"
#include "../../../iot_link/os/os_imp/liteos/include/los_base.h"
#include "../../../iot_link/os/os_imp/liteos/include/los_config.h"
#include "../../../iot_link/os/os_imp/liteos/include/los_swtmr.h"
#include "../../../iot_link/os/os_imp/liteos/include/los_sys.h"
#include "../../../iot_link/os/os_imp/liteos/include/los_task.h"
#include "../../../iot_link/os/os_imp/liteos/include/los_typedef.h"
#include "gd32vf103.h"

#include "usart.h"
#include "wifitask.h"
#include "timer.h"
#include "sht30.h"

//帧格式长度
#define TERMINAL_ID_HEAD_IDX  0
#define SENSOR_TYPE_IDX  1
#define SOURCEIP_IDX  2
#define Data_IDX 4
#define SendQueueLength 10			//发送队列长度
#define APPdataLength 100

//帧格式定义
#define sensor_frame_length  30
#define TerminalID  0x01
#define Tem_sentor_head_id  0x01
#define smoke_sentor_head_id  0x02
#define co_sentor_head_id  0x03
#define led_head_id 0x01

//定义发送队列
typedef struct SendQueue
{
	uint8_t sendBuf[SendQueueLength][50];
	uint8_t nextStorage;			//指向存储位置（队尾）
	uint8_t currentSendpos;			//指向当前需要发送的buf（队头）
	uint8_t currentNum;				//表示队列内的待发送数量
}sendQueue;


extern UINT32 sensor_function_start;
extern uint8_t sensor_frame[sensor_frame_length];

int all_tasks_entry(void);
void device_init(void);

static void data_frame_analyse_function(uint8_t *payload, uint8_t* APPmode,uint8_t* APPdata);
static void ModifyCollectFre(UINT32 fre);
static void Designated_sensor_analyse(UINT32 sensor_id);

void ClearSendQueue(sendQueue *Queue);
void wifi_send_function(uint8_t* sourceip, uint8_t* targetip, uint8_t len, uint8_t mode, uint8_t* data, sendQueue *WiFisendQueue);


#endif /* TARGETS_GD32VF103V_EVAL_INC_LITEOS_ALL_TASKS_H_ */
