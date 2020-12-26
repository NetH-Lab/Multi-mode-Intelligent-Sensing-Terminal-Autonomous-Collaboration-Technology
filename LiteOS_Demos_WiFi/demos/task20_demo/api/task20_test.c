#include <stdio.h>
#include <stdlib.h>
#include "los_base.h"
#include "los_typedef.h"
#include "los_task.h"
#include "task20_test.h"

#define random(x) (rand()%x)
#define println printf
#define max_num_of_task 23
#define weight_per_task 1000
#define hz_per_task 10 * 2
#define lv_1 6
#define lv_2 8
#define lv_3 7

static UINT32 g_TaskID[max_num_of_task];
static UINT32 temp_res;
static UINT32 total_run_count = 0;
static UINT32 total_sec = 0;

void task_entry_01(void)
{
    while (1)
    {
        gpio_bit_write(GPIOC, GPIO_PIN_12, 0);
        uint32_t co = 0;
        uint32_t max = 1000;
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;
        gpio_bit_write(GPIOC, GPIO_PIN_12, 1);
        //return;   
        
        LOS_TaskDelay(10);     
    }
}

void run_test_task_01(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_01;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test1";
    stTaskInitParam.usTaskPrio = lv_1;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_02(void)
{
    while (1)
    {        
        gpio_bit_write(GPIOD, GPIO_PIN_2, 0);
        uint32_t sum = 0;
        uint32_t cnt = 1000;
        for (uint32_t i = 0; i < cnt; i++)
        {
            sum+=i;
        }
        temp_res = sum;
        gpio_bit_write(GPIOD, GPIO_PIN_2, 1);
        LOS_TaskDelay(1);
    }
}

void run_test_task_02(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_02;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test2";
    stTaskInitParam.usTaskPrio = lv_2;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_03(void)
{
    while (1)
    {
        total_sec++;
        uint32_t speed = total_run_count * 100 / total_sec;
        uint32_t left = speed / 100;
        uint32_t right = speed % 100;
//        printf("task run times per sec : %d.%d\r\n", left, right);
//        printf(".");
        LOS_TaskDelay(1000);
    }
}

void run_test_task_03(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_03;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test3";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_04(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_04(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_04;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test4";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_05(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_05(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_05;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test5";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_06(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_06(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_06;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test6";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_07(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_07(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_07;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test7";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_08(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_08(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_08;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test8";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_09(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_09(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_09;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test9";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_10(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_10(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_10;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test10";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_11(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_11(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_11;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test11";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_12(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_12(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_12;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test12";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_13(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_13(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_13;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test13";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_14(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_14(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_14;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test14";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_15(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_15(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_15;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test15";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_16(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_16(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_16;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test16";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_17(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_17(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_17;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test17";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_18(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_18(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_18;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test18";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_19(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_19(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_19;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test19";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_20(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_20(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_20;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test20";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_21(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_21(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_21;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test21";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_22(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_22(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_22;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test22";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void task_entry_23(void)
{
    while (1)
    {
        uint32_t co = 0;
        uint32_t max = weight_per_task + random(weight_per_task);
        for (uint32_t i = 0; i < max; i++)
        {
            co+=i;
        }
        temp_res = co;     
        total_run_count++;
        LOS_TaskDelay(random(hz_per_task));
    }
}

void run_test_task_23(UINT32 no)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)task_entry_23;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test23";
    stTaskInitParam.usTaskPrio = lv_3;
    uwRet = LOS_TaskCreate(&g_TaskID[no-1], &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
}

//==============================================================================================

void run20tasks(void)
{
    gpio_bit_write(GPIOD, GPIO_PIN_2, 1);
    delay_1ms(100*10*5);
    run_test_task_01(1);//优先级1
    run_test_task_02(2);//优先级2
//    run_test_task_03(3);//计算每秒任务运行数

  /*  run_test_task_04(4);//优先级3
   run_test_task_05(5);
    run_test_task_06(6);
    run_test_task_07(7);
    run_test_task_08(8);

    run_test_task_09(9);
    run_test_task_10(10);
    run_test_task_11(11);
    run_test_task_12(12);
    run_test_task_13(13);

    run_test_task_14(14);
    run_test_task_15(15);
    run_test_task_16(16);
    run_test_task_17(17);
    run_test_task_18(18);

     run_test_task_19(19);
    run_test_task_20(20);
    run_test_task_21(21);
    run_test_task_22(22);
    run_test_task_23(23);*/
}
