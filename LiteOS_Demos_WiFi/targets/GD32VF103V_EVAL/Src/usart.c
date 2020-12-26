/*
 * usart.c
 *
 *  Created on: 2020��12��6��
 *      Author: minel
 */

#include "usart.h"
#include <stdio.h>

uint8_t UART3_RX_BUF[RX_BUF_SIZE];
uint16_t UART3_RX_STA = 0;		/* UART3 receive flag; 1: data has been received */
uint8_t UART4_RX_BUF[RX_BUF_SIZE];
uint16_t UART4_RX_STA = 0;		/* UART4 receive flag; 1: data has been received */

static UINT32 g_timercount1 = 0;
UINT16 uart3_timer;
uint8_t uart3_timer_enable = 0;
uint8_t uart3_timer_count = 0;
uint8_t sensor_data[sensor_data_length];

extern uint16_t PeriodUpdateCounter;
extern uint8_t PeriodUpdateEnable;

extern uint16_t SendCounter;	//���Ϳ��Ƽ�ʱ�������ڵ��ڷ���Ƶ��

/*
 * 	Function: get usart receive
 * 	Parameter In:
 * 		*receivebuf: ����bufָ��
 * 		len: data length
 * 	Parameter Out: receivebuf
 */
uint8_t* get_usart_recv(uint16_t data_length, uint8_t* receivebuf)
{
	uint8_t i, *p = receivebuf;
	for(i=0; i<data_length; i++)
	{
		*p = UART4_RX_BUF[i];
		p++;
	}
	*p = '\0';
	return receivebuf;
}

/*
 * 	Function: usart initialization
 * 	Parameter In:
 * 		bound: ������
 * 	Parameter Out: None
 */
void uart2_init(unsigned int bound)
{
	/* enable GPIO clock */
	rcu_periph_clock_enable(RCU_GPIOB);
	/* enable USART clock */
	rcu_periph_clock_enable(RCU_USART2);

	/* connect port to USARTx_Tx */
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_10);

	/* connect port to USARTx_Rx */
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_11);

	/* USART configure */
	usart_deinit(USART2);
	usart_baudrate_set(USART2, bound);
	usart_word_length_set(USART2, USART_WL_8BIT);
	usart_stop_bit_set(USART2, USART_STB_1BIT);
	usart_parity_config(USART2, USART_PM_NONE);
	usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
	usart_receive_config(USART2, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
	usart_enable(USART2);
}
/*
 * 	Function: usart initialization
 * 	Parameter In:
 * 		bound: ������
 * 	Parameter Out: None
 */
void uart3_init(unsigned int bound)
{
	/* enable GPIO clock */
	rcu_periph_clock_enable(RCU_GPIOC);
	/* enable USART clock */
	rcu_periph_clock_enable(RCU_UART3);

	/* connect port to USARTx_Tx */
	gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_10);

	/* connect port to USARTx_Rx */
	gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_11);

	/* USART configure */
	usart_deinit(UART3);
	usart_baudrate_set(UART3, bound);
	usart_word_length_set(UART3, USART_WL_8BIT);
	usart_stop_bit_set(UART3, USART_STB_1BIT);
	usart_parity_config(UART3, USART_PM_NONE);
	usart_hardware_flow_rts_config(UART3, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(UART3, USART_CTS_DISABLE);
	usart_receive_config(UART3, USART_RECEIVE_ENABLE);
	usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);
	usart_enable(UART3);
}

/*
 * 	Function: usart initialization
 * 	Parameter In:
 * 		bound: ������
 * 	Parameter Out: None
 */
void uart4_init(unsigned int bound)
{
	/* enable GPIO clock */
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	/* enable USART clock */
	rcu_periph_clock_enable(RCU_UART4);

	/* connect port to USARTx_Tx */
	gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_12);

	/* connect port to USARTx_Rx */
	gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_2);

	/* USART configure */
	usart_deinit(UART4);
	usart_baudrate_set(UART4, bound);
	usart_word_length_set(UART4, USART_WL_8BIT);
	usart_stop_bit_set(UART4, USART_STB_1BIT);
	usart_parity_config(UART4, USART_PM_NONE);
	usart_hardware_flow_rts_config(UART4, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(UART4, USART_CTS_DISABLE);
	usart_receive_config(UART4, USART_RECEIVE_ENABLE);
	usart_transmit_config(UART4, USART_TRANSMIT_ENABLE);
	usart_enable(UART4);
}

/*
 * 	Function: UART3 Send Function
 * 	Parameter In:
 * 		*ch: string
 * 	Parameter Out: None
 */
void u3_printf(char* ch)
{
	while(*ch != '\0')
	{
		usart_data_transmit(UART3, *ch);
		while ( usart_flag_get(UART3, USART_FLAG_TBE)== RESET){};
		ch++;
	}
}

static void USART2_Exti_Init()
{
    /*add your IRQ init code here*/
	usart_interrupt_enable(USART2, USART_INT_RBNE);
    return;
}

/*
 * 	Function: get smoke_sensor data
 * 	Parameter In:None
 * 	Parameter Out: None
 */

static void UART2IRQ(void)
{
	uint8_t ch,smoke_data;
	static uint8_t count = 0;
	static uint8_t data_len_count = 0;
	static uint8_t data[2];
	if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE) != RESET)
	{
		usart_interrupt_flag_clear(USART2, USART_FLAG_RBNE);
		ch = (uint8_t)usart_data_receive(USART2);
        switch (count)
        {
        case 0:
        {
        	 if (ch == 0x01)
                count++;
            else
                count = 0;
        }break;
        case 1:
        {
        	if (ch == 0x03)
                count++;
            else
                count = 0;
        } break;
        case 2:
        {
        	if (ch == 0x02)
                count++;
            else
                count = 0;
        }break;
        case 3:
        {
            data[data_len_count] = ch;
            data_len_count++;
            if (data_len_count >= 2)
            {
            	smoke_data=(data[0]<<8)+data[1];

            	//��smoke_dataת�����ַ���
                sprintf(sensor_data,"%d",smoke_data);
                printf("sensor_data=%s\r\n\r\n", sensor_data);
            	data_len_count = 0;
                count++;
            }
        } break;
        default:
            count = 0;
            break;
        }
	}
	return;
}

/*
 * 	Function: get co_sensor data
 * 	Parameter In:None
 * 	Parameter Out: None
 */
//static void UART2IRQ()
//{
//	uint8_t ch,co_data;
//	static uint8_t count = 0;
//	static uint8_t data_len_count = 0;
//	static uint8_t data[2];
//	if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE) != RESET)
//	{
//		usart_interrupt_flag_clear(USART2, USART_FLAG_RBNE);
//		ch = (uint8_t)usart_data_receive(USART2);
//        switch (count)
//        {
//        case 0:
//        {
//        	 if (ch == 0x01)
//        	    count++;
//            else
//                count = 0;
//        }break;
//        case 1:
//        {
//        	if (ch == 0x03)
//        	    count++;
//            else
//                count = 0;
//        } break;
//        case 2:
//        {
//        	if (ch == 0x02)
//        	    count++;
//            else
//                count = 0;
//        }break;
//        case 3:
//        {
//            data[data_len_count] = ch;
//            data_len_count++;
//            if (data_len_count >= 2)
//            {
//            	co_data=(data[0]<<8)+data[1];
//              printf("co_data=%d\r\n\r\n", co_data);

//            //��co_dataת�����ַ���
//             sprintf(sensor_data,"%d",co_data);
//            	data_len_count = 0;
//                count++;
//            }
//        } break;
//        default:
//            count = 0;
//            break;
//        }
//	}
//	return;
//}


UINT32 UART2_Interrupt(VOID)
{
    UINTPTR uvIntSave;
    uvIntSave = LOS_IntLock();

    USART2_Exti_Init();

    LOS_HwiCreate(USART2_IRQn, 0, 0, UART2IRQ, 0); //Create Interruption

    LOS_IntRestore(uvIntSave);

    return LOS_OK;
}

static void UART3_Exti_Init()
{
    /*add your IRQ init code here*/
	usart_interrupt_enable(UART3, USART_INT_RBNE);
    return;

}

static VOID UART3IRQ(void)
{
	uint8_t Res;
	if(usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE) != RESET)	//Receive Interruption (�������ݱ�����0x0d 0x0a��β)
	{
		Res = usart_data_receive(UART3);	//����һ���ַ�
		if(UART3_RX_STA < RX_BUF_SIZE)		//����δ��ɣ��ҽ���buf�������пռ�
		{
			uart3_timer_count = 0;	//���������
			if(UART3_RX_STA == 0)	//���յ���һ���ַ�
			{
				uart3_timer_count = 0;	//���������
				uart3_timer_enable = 1;	//������ʱ��, 1s��δ���յ������ַ�����ɽ���
			}
			UART3_RX_BUF[UART3_RX_STA++]=Res;	//��¼���յ���ֵ
		}
		else
			UART3_RX_STA|=1<<15;		//ǿ�Ʊ�ǽ������

	}
    return;
}

UINT32 UART3_Interrupt(VOID)
{
    UINTPTR uvIntSave;
    uvIntSave = LOS_IntLock();

    UART3_Exti_Init();

    LOS_HwiCreate(UART3_IRQn, 1, 0, UART3IRQ, 0); //Create Interruption

    LOS_IntRestore(uvIntSave);

    return LOS_OK;
}


static void UART4_Exti_Init()
{
    /*add your IRQ init code here*/
	usart_interrupt_enable(UART4, USART_INT_RBNE);
    return;

}

static VOID UART4IRQ(void)
{
	uint8_t Res;
	if(usart_interrupt_flag_get(UART4, USART_INT_FLAG_RBNE) != RESET)	//Receive Interruption (�������ݱ�����0x0d 0x0a��β)
	{
		Res = usart_data_receive(UART4);	//����һ���ַ�
		if((UART4_RX_STA&0x8000) == 0)		//����δ���
		{
			if(UART4_RX_STA&0x4000)			//��һ�ν��ܵ���0x0d, ����Ӧ�ý���0x0a
			{
				if(Res != 0x0a) UART4_RX_STA = 0;	//�˴ν��ܴ������½���
				else UART4_RX_STA |= 0x8000;		//���Ϊ�������
			}
			else	//��δ���յ�0x0d
			{
				if(Res == 0x0d) UART4_RX_STA |= 0x4000;
				else
				{
					UART4_RX_BUF[UART4_RX_STA&0X3FFF]=Res;
					UART4_RX_STA++;
					if(UART4_RX_STA>(RX_BUF_SIZE-1)) UART4_RX_STA=0;	//���ݳ����Ѵ��ڽ���buf���ȣ�������ܴ���Flag��0�����¿�ʼ����
				}
			}
		}
	}
    return;
}

UINT32 UART4_Interrupt(VOID)
{
    UINTPTR uvIntSave;
    uvIntSave = LOS_IntLock();

    UART4_Exti_Init();

    LOS_HwiCreate(UART4_IRQn, 0, 0, UART4IRQ, 0); //Create Interruption

    LOS_IntRestore(uvIntSave);

    return LOS_OK;
}

/*
 * 	Function: Timer1 Callback Function
 * 	Parameter In:
 * 		*ch: string
 * 	Parameter Out: None
 */

VOID Timer_uart3_Callback(UINT32 arg)
{
	if(uart3_timer_enable && uart3_timer_count > 5)		//ʹ�ܣ����Ҽ���������5 (500ms)
	{
		UART3_RX_STA|=1<<15;	//��ǽ������
		uart3_timer_enable = 0;	//�رն�ʱ��
		uart3_timer_count = 0;	//����
	}
	else if(uart3_timer_enable)
		uart3_timer_count++;
	if(PeriodUpdateEnable && PeriodUpdateCounter < 255)
	{
		PeriodUpdateCounter++;
	}
	if(PeriodUpdateEnable && SendCounter < 255)
	{
		SendCounter++;
	}

}

UINT32 UART3_swTimer(VOID)
{
	UINT32 uwRet = LOS_OK;
	uwRet = LOS_SwtmrCreate(100, LOS_SWTMR_MODE_PERIOD, Timer_uart3_Callback, &uart3_timer, 1, OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_SENSITIVE);
}




