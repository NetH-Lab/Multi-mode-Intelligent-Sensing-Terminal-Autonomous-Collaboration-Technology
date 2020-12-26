#include <stdio.h>

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
#include "wificommand.h"
#include "router.h"




#if defined (__CC_ARM)
extern char __heap_start__ [];
#elif defined (__GNUC__)
extern char __los_heap_addr_start__ [];
extern char __los_heap_addr_end__ [];
#else
#error "fix me"
#endif

extern VOID LOS_MemInfo (VOID * pPool, bool bShowDetail);
extern int link_main(void *args);


const struct phys_mem system_phys_mem [] =
    {
#if defined (__CC_ARM)
        { __heap_start__, (char *) 0x2002FC00, },
#elif defined (__GNUC__)
        {(unsigned long)__los_heap_addr_start__, (unsigned long)__los_heap_addr_end__, },
#else
#error "fix me"
#endif
        { 0, 0 }
    };

int main() {
	/* Initiation Functions */
	device_init();
	delay_1ms(1000);

    UINT32 uwRet = LOS_OK;
    /* init OS kernel */
    uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
    delay_1ms(1000);
    /* Create all tasks */
    uwRet = all_tasks_entry();
   if(uwRet != LOS_OK)
   {
	return LOS_NOK;
   }
   delay_1ms(1000);
    /* Create all interruptions */
    UART2_Interrupt();
    UART3_Interrupt();
    UART4_Interrupt();
    delay_1ms(1000);

    /* Create all timers */
    Led_Sensor_swTimer(1000);
    UART3_swTimer();
    LOS_SwtmrStart(uart3_timer);
    delay_1ms(1000);

    (void)LOS_Start();
    while(1){};
    return 0;
}
