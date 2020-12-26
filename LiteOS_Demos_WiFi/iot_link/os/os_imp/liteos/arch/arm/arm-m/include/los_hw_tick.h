/*----------------------------------------------------------------------------
 * Copyright (c) <2013-2015>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

 /**@defgroup los_hw hardware
   *@ingroup kernel
 */

#ifndef _LOS_HW_TICK_H
#define _LOS_HW_TICK_H

#include "los_base.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if (LOSCFG_KERNEL_TICKLESS == YES)
/**
 * @ingroup los_hwi
 * Check whether the counting direction of system tick is decreasing, it will be used to
 * readjust the value of the system tick, if not decreasing, please set this macro to NO.
 */
#define LOSCFG_SYSTICK_CNT_DIR_DECREASE      YES

/**
 * @ingroup los_hwi
 * Max reload value of system tick.
 */
#define LOSCFG_SYSTICK_LOAD_RELOAD_MAX      SysTick_LOAD_RELOAD_Msk

/*****************************************************************************
Function   : LOS_SysTickStop
Description: stop systick
Input   : none
output  : none
return  : none
*****************************************************************************/
LITE_OS_SEC_TEXT_MINOR static inline VOID LOS_SysTickStop(VOID)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/*****************************************************************************
Function   : LOS_SysTickStart
Description: start systick
Input   : none
output  : none
return  : none
*****************************************************************************/
LITE_OS_SEC_TEXT_MINOR static inline VOID LOS_SysTickStart(VOID)
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/*****************************************************************************
Function   : LOS_SysTickGetIntStatus
Description: get systick interrupt status
Input   : none
output  : none
return  : systick interrupt status
*****************************************************************************/
LITE_OS_SEC_TEXT_MINOR static inline UINT32 LOS_SysTickGetIntStatus(VOID)
{
    return SCB->ICSR & SCB_ICSR_PENDSTSET_Msk;
}

/**
 *@ingroup los_hwi
 *@brief Get value from xPSR register.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to Get value from xPSR register.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>None.</li>
 *</ul>
 *
 *@param None.
 *
 *@retval xPSR register value.
 *@par Dependency:
 *<ul><li>los_hwi.h: the header file that contains the API declaration.</li></
ul>
 *@see LOS_IntRestore
 *@since Huawei LiteOS V100R001C00
 */
extern VOID LOS_GetCpuCycle(UINT32 *puwCntHi, UINT32 *puwCntLo);

extern UINT32 LOS_SysTickCurrCycleGet(VOID);

extern VOID LOS_SysTickReload(UINT32 uwCyclesPerTick);

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


#endif /* _LOS_HW_H */

