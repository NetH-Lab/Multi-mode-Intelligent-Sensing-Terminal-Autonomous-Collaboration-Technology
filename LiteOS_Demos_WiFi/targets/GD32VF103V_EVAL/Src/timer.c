/*
 * timer.c
 *
 *  Created on: 2020年12月9日
 *      Author: Administrator
 */
#include "timer.h"

static void Timer_Callback    (UINT32 arg);   // callback fuction

static UINT32 g_timercount1 = 0;
UINT32 sensor_function_start_co = 0;
UINT32 sensor_function_start_smoke = 0;
UINT32 sensor_function_start_temp = 0;
UINT16 timer_id;// timer id

static void Timer_Callback(UINT32 arg)//回调函数1
{
      if(g_timercount1==1)
      {
    	  gpio_bit_set(GPIOA,GPIO_PIN_1);
    	  g_timercount1=0;
      }
      else {
    	  gpio_bit_reset(GPIOA,GPIO_PIN_1);
    	  g_timercount1=1;
      }
      if(sensor_function_start_co < 255)
    	  sensor_function_start_co++;
      if(sensor_function_start_smoke < 255)
    	  sensor_function_start_smoke++;
      if(sensor_function_start_temp < 255)
    	  sensor_function_start_temp++;
}

/*
 * 	Function: Led_Sensor_swTimer
 * 	Parameter In:fre
 * 	Parameter Out: None
 */
UINT32 Led_Sensor_swTimer(uint16_t fre)
{

    UINT32 uwRet = LOS_OK;

    /*创建周期性软件定时器，每500Tick数执行回调函数2 */
    uwRet =LOS_SwtmrCreate(fre, LOS_SWTMR_MODE_PERIOD, Timer_Callback, &timer_id, 1, OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_SENSITIVE);
    if(LOS_OK != uwRet)
    {
        dprintf("create Timer failed\n");
    }
    else
    {
        dprintf("create Timer success\n");
    }
    uwRet = LOS_SwtmrStart(timer_id);//启动周期性软件定时器
    if(LOS_OK != uwRet)
    {
        dprintf("start Timer failed\n");
    }
    else
    {
        dprintf("start Timer success\n");
    }
    return LOS_OK;
}



