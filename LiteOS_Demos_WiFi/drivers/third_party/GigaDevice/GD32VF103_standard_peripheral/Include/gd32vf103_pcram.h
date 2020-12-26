#ifndef GD32VF103_PCRAM_H
#define GD32VF103_PCRAM_H

#include <stdio.h>
#include "gd32vf103.h"
#include "systick.h"
#include "gd25qxx.h"
#include "gd32v103v_eval.h"


#define Bank0_SRAM2_ADDR            ((uint32_t)0x60000000)
#define BANK_SRAM_ADDR              Bank0_SRAM2_ADDR

void exmc_pcram_init(void);

#endif /* GD32VF103_PCRAM_H */