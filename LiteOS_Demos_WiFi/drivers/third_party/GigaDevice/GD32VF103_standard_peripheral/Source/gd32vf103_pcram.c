/*!
    \file  main.c
    \brief SPI flash demo
    
    \version 2019-06-05, V1.0.0, demo for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32vf103_pcram.h"

void exmc_pcram_init(void)
{
    exmc_norsram_parameter_struct nor_init_struct;
    exmc_norsram_timing_parameter_struct nor_timing_init_struct;
    uint32_t cfg_data = 0xC0DB4CB9;//0xC0DB4CB9;
    int i;
    bit_status bit_value;

    /* EXMC clock enable */
    rcu_periph_clock_enable(RCU_EXMC);
    rcu_periph_clock_enable(RCU_AF);

    /* EXMC enable */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);

    /* configure EXMC_D[0~15]*/
    /* PD14(EXMC_D0), PD15(EXMC_D1),PD0(EXMC_D2), PD1(EXMC_D3), PD8(EXMC_D13), PD9(EXMC_D14), PD10(EXMC_D15) */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1| GPIO_PIN_8 | GPIO_PIN_9 |
                                                         GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);

    /* PE7(EXMC_D4), PE8(EXMC_D5), PE9(EXMC_D6), PE10(EXMC_D7), PE11(EXMC_D8), PE12(EXMC_D9),
       PE13(EXMC_D10), PE14(EXMC_D11), PE15(EXMC_D12) */
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
                                                         GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                                                         GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    /* PD11(EXMC_A16), PD12(EXMC_A17), PD13(EXMC_A18) */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11 | GPIO_PIN_12);// | GPIO_PIN_13);

    /* configure NOE(PD4),NWE(PD5) and NE0(PD7) */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);

    /* configure NBL0(PE0) and NBL1(PE1) */
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);

    /* configure EXMC NADV (PB7) */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    /* configure timing parameter */
    nor_timing_init_struct.bus_latency = 0x0F;//1;
    //nor_timing_init_struct.asyn_data_setuptime = 0x0ff;//7;
    nor_timing_init_struct.asyn_data_setuptime = 0x0FF;//7;
    nor_timing_init_struct.asyn_address_holdtime = 0x01;//2;
    nor_timing_init_struct.asyn_address_setuptime = 0x0F;//5;

    /* configure EXMC bus parameters */
    nor_init_struct.norsram_region = EXMC_BANK0_NORSRAM_REGION0;
    nor_init_struct.asyn_wait = DISABLE;//DISABLE;��֪��Ϊʲôdisable�첽�ȴ�����Ч
    //nor_init_struct.asyn_wait = ENABLE;//ENABLE;
    nor_init_struct.nwait_signal = DISABLE;
    nor_init_struct.memory_write = ENABLE;
    nor_init_struct.nwait_polarity = EXMC_NWAIT_POLARITY_LOW;
    nor_init_struct.databus_width = EXMC_NOR_DATABUS_WIDTH_16B;
    nor_init_struct.memory_type = EXMC_MEMORY_TYPE_NOR;//EXMC_MEMORY_TYPE_NOR;//EXMC_MEMORY_TYPE_SRAM;//EXMC_MEMORY_TYPE_PSRAM
    nor_init_struct.address_data_mux = ENABLE;//DISABLE;//ENABLE;
    nor_init_struct.read_write_timing = &nor_timing_init_struct;
    exmc_norsram_init(&nor_init_struct);

    /* enable the EXMC bank0 NORSRAM */
    exmc_norsram_enable(EXMC_BANK0_NORSRAM_REGION0);

  //  gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_7);
  //  gpio_bit_set(GPIOD, GPIO_PIN_7);

	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2); //CFGC
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6); //CFGD
	gpio_bit_write(GPIOB, GPIO_PIN_2, 0);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9); //LED_RUN
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0); //ZZ

	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8); //EXMC_CE

	gpio_bit_write(GPIOC, GPIO_PIN_9, 0);//LED_RUN
	gpio_bit_write(GPIOB, GPIO_PIN_0, 1);//ZZ
	delay_1ms(1);
	/////////////////////////////����PCRAM����

	for(i=0;i<32;i++)
	{
		delay_1ms(1);
		bit_value = (bit_status)((cfg_data >> (31-i)) & 0x01);
		gpio_bit_write(GPIOB, GPIO_PIN_6, (bit_status)bit_value);
		delay_1ms(1);
		gpio_bit_write(GPIOB, GPIO_PIN_2,(bit_status)(1));
		delay_1ms(2);
		gpio_bit_write(GPIOB, GPIO_PIN_2,(bit_status)(0));
	}

	/////////////////////////////
	gpio_bit_write(GPIOB, GPIO_PIN_0, 1);//ZZ
	gpio_bit_write(GPIOC, GPIO_PIN_8, 0);//EXMC_CE
	gpio_bit_write(GPIOB, GPIO_PIN_6, (bit_status)1);
}



