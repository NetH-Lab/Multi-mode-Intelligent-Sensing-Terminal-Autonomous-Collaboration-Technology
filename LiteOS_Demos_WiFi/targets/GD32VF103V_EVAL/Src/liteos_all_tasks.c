#include "liteos_all_tasks.h"


extern uint8_t WIFI_RECV_STA;		//���ܱ�־λ��Ϊ1��������Ϣ��Ҫ������
extern uint8_t WIFI_RECV_BUF[WIFI_TX_BUF_SIZE];
extern uint16_t PeriodUpdateCounter;
extern uint8_t PeriodUpdateEnable;
extern uint16_t SendCounter;	//���Ϳ��Ƽ�ʱ�������ڵ��ڷ���Ƶ��

static uint8_t smoke_register[8]={0x01,0x03,0x00,0x0B,0x00,0x01,0xF5,0xC8};
static uint8_t co_register[8]={0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0A};
uint8_t sensor_frame[sensor_frame_length];

static uint8_t Designated_co_sensor_upload = 0;
static uint8_t Designated_temp_sensor_upload = 0;
static uint8_t Designated_smoke_sensor_upload = 0;

/*
 * 	Function: char_to_int
 * 	Parameter In:
 *	*data: char type data
 * 	Parameter Out:int type data
 */
int char_to_int(uint8_t *data)
{
    int i, j = 0, k = 1;
    for (i = 0; data[i]; i++)
        data[i] -= '0';
    i--;
    for (; i >= 0; i--)
    {
        j += data[i] * k;
        k *= 10;
    }
    return j;
}

/*
 * 	Function: Data frame generate Function. Before using this function, you need check if WIFI_SEND_STA is 1 or not.
 * 	Parameter In:
 * 		terminalID: 	terminal IP
 *      sensor_type: terminnal sensor type
 * 		*sourceip: 	 source IP
 *		*data:		data for terminnal
 * 	Parameter Out: 	None
 */
void data_frame_generate_function(uint8_t terminalID, uint8_t sensor_type, uint8_t executor_type, uint8_t *data)
{
    uint8_t *pos = sensor_frame, i, datalen;
    *pos++ = terminalID;
    *pos++ = sensor_type;
    *pos++ = executor_type;
    datalen = strlen(data); //��ȡ���ݳ���
    memcpy(pos, data, datalen);
    *(pos + datalen) = '\0';
}

/*
 * 	Function: send_sensor_register
 * 	Parameter In:None
 * 	Parameter Out: None
 */
void send_sensor_register(uint8_t *data) {
	for (int i=0; i < 8; i++) {
		usart_data_transmit(USART2, data[i]);
		delay_1ms(10);
	}
}

/*
 * 	Function: WiFi_task_entry
 * 	Parameter In:None
 * 	Parameter Out: None
 */

LITE_OS_SEC_TEXT VOID WiFi_task_entry(void)
{
	/* -----------------------------------------------Network layer paramaters define------------------------------------------------- */
	sendQueue WiFisendQueue;			//����һ�����Ͷ���
	uint8_t DEVICE_STATE = 0;		//The state of device. 0: device has not connected with the sink node; 1: device has connected with the sink node.
    uint8_t PeriodUpdatePacket[10] = "Hello", len, mode, LinkID;
    uint8_t tcp_demo_sendbuf[255];
    char recvFlag[5] = "+IPD";
    recvFlag[4] = '\0';
    UINTPTR uvIntSave;
    UINT32 uwRet = LOS_OK;
    /* -----------------------------------------------APP paramaters define------------------------------------------------------------ */
    UINT32 fre;
    float Tem_Hum[2];
    char* str = 0;
    uint8_t APPmode, APPdata[APPdataLength];

    ClearSendQueue(&WiFisendQueue);		//��ʼ������
    while(1)
    {
    	LOS_TaskDelay(2000);
        if(!DEVICE_STATE)
        {
        	/* Communication Initialization */
        	printf("Network Initialization......\r\n");
        	LOS_TaskDelay(100);
        	while(WIFIInit());						//Ad-hoc networking initialization & Get
        	InitRouteList();
        	CreatTCP('1');
        	DEVICE_STATE = 1;
        }
        else
        {
        	ClearSendQueue(&WiFisendQueue);		//��ʼ������
        	PeriodUpdateEnable = 1;			//Enable period update function
        	UART3_RX_STA = 0;
            while(1)
            {
            	LOS_TaskDelay(1000);
            	/* -------------------------------------------------------------------------------------------------------------- */
                /*Receiving child thread*/
                if(UART3_RX_STA & 0x8000)		//���յ�һ��������
                {
                	 if(atk_8266_check_cmd("CLOSED"))
					{
						printf("WiFi has disconnected\r\n");
						DEVICE_STATE = 0;
						break;
					}
                	str = strstr((const char*)UART3_RX_BUF, recvFlag);
                	if(str)
                	{
                		LinkID = *(str + ESP8266_LINKID_IDX);
						mode = *(str + ESP8266_DATA_HEAD_IDX + MODE_IDX) & 0XF0;
						len = *(str + ESP8266_DATA_HEAD_IDX + DataLen_IDX);
						memcpy(WIFI_RECV_BUF, str + ESP8266_DATA_HEAD_IDX + DATAHEAD_IDX, len - DATAHEAD_IDX);	//�����ݿ�����WIFI_RECV_BUF��
						WIFI_RECV_BUF[len - DATAHEAD_IDX] = '\0';
						printf("���ν��ܵ�����Ϊ��%s\r\n", WIFI_RECV_BUF);
//						switch(mode)
//						{
//							case
//						}
						WIFI_RECV_STA = 1;			//��־�����ݴ�����
                	}
                	UART3_RX_STA = 0;				//������һ�����ݽ���
                }
                /* -------------------------------------------------------------------------------------------------------------- */

				/* -------------------------------------------------------------------------------------------------------------- */
//              /*uart2 Receiving thread*/
				if (WIFI_RECV_STA)
				{
					//���ý��պ������õ�Ƶ��
//					printf("WIFI_RECV_BUF = %s\r\n",WIFI_RECV_BUF); //WIFI_RECV_BUF is receiva data from wifi
					data_frame_analyse_function(WIFI_RECV_BUF, &APPmode, APPdata);
					//����֡��������
					fre = (UINT32)atof(APPdata);		//modeΪ1ʱ����ɼ�Ƶ�ʣ�modeΪ2ʱ�����������
//					printf("APPmode: %d, Fre: %d", APPmode, fre);
//					switch(APPmode)
//					{
//						case 1: ModifyCollectFre(fre); break;
//						case 2: Designated_sensor_analyse(fre); break;
//						default: printf("APP mode error\r\n");
//					}
					WIFI_RECV_STA = 0;
					continue;
				}
				/* -------------------------------------------------------------------------------------------------------------- */

                /* -------------------------------------------------------------------------------------------------------------- */
                /* Sending child thread*/
                if(SendCounter > 20 && WiFisendQueue.currentNum)		//�ж��Ƿ���Ҫ���ͣ�ʹ��SendCounter���Ʒ���Ƶ�ʣ�Ƶ����ÿ�����2s����
                {
                	len = WiFisendQueue.sendBuf[WiFisendQueue.currentSendpos][DataLen_IDX];		//��ȡ�����ܳ���
                	mode = WiFisendQueue.sendBuf[WiFisendQueue.currentSendpos][MODE_IDX]&0x0F;	//��ȡ���ͷ�ʽ, 0-��̫��udp, 1-��̫��tcp, 2-WIFIup, 3-WIFIdown
                	memcpy(tcp_demo_sendbuf, WiFisendQueue.sendBuf[WiFisendQueue.currentSendpos], len);	//�������ݵ�����buf
                	tcp_demo_sendbuf[len] = '\0';
                	printf("len: %d; mode: %d\r\n", len, mode);
                	printf("�˴η��͵�����Ϊ: %s\r\n", tcp_demo_sendbuf);
                	switch(mode&0x0F)						//�жϷ�������
                	{
//                		case ETH_UDP:
//                			udp_demo_test(tcp_demo_sendbuf);
//                			break;
//                		case ETH_TCP:
//                			tcp_client_test(tcp_demo_sendbuf);
//                			break;
                		case WIFIUP:
                			TCPtransmitUP(tcp_demo_sendbuf, len);
                			break;
                		case WIFIDOWN:
                			TCPtransmitDOWN(tcp_demo_sendbuf, len);
                			break;
                		default:
                			printf("Send Mode Error\n");
                			break;
                	}
                	UART3_RX_STA = 0;		//��wifiģ�鷢������ʱ��ʹ��λ��1���ʷ��ͽ�������wifiģ�����ݽ���
                	/*���ͽ�������¶��е�ֵ*/
                	WiFisendQueue.currentNum--;
                	WiFisendQueue.currentSendpos++;
                	if(WiFisendQueue.currentSendpos >= SendQueueLength)	//����ͷ�Ƿ����
                		WiFisendQueue.currentSendpos = 0;
                	SendCounter = 0;		//������һ�η��ͼ�ʱ
                	continue;
                }
                /* -------------------------------------------------------------------------------------------------------------- */

                /* -------------------------------------------------------------------------------------------------------------- */
				/*Period update child thread*/
				if(PeriodUpdateCounter > 100)	//��ʱ���������ڷ��ͱ��ļ��
				{
					wifi_send_function(staip, remoteip, 15, 0x02, PeriodUpdatePacket, &WiFisendQueue);
					PeriodUpdateCounter = 0;
					continue;
				}
				/* -------------------------------------------------------------------------------------------------------------- */

				/* -------------------------------------------------------------------------------------------------------------- */
                /*co_sensor thread*/
//                if(sensor_function_start_co > 3 || Designated_co_sensor_upload)
//                {
//                	LOS_TaskDelay(50);
//                	send_sensor_register(co_register);
//                    //����֡�ϳɺ���
//					sensor_data[0] = '1';
//					sensor_data[1] = '2';
//					sensor_data[2] = '\0';
//					data_frame_generate_function(0x03, co_sentor_head_id, led_head_id, sensor_data);
//                    //data_frame_generate_function(TerminalID, co_sentor_head_id, led_head_id, sensor_data);
//					//���÷��ͺ���������sensor_frame
//					wifi_send_function(staip, remoteip, 13+strlen(sensor_data), 0x22, sensor_frame, &WiFisendQueue);
//					sensor_function_start_co = 0;
//					Designated_co_sensor_upload = 0;
//					continue;
//                }
				/* -------------------------------------------------------------------------------------------------------------- */

				/* -------------------------------------------------------------------------------------------------------------- */
                /*smoke_sensor thread*/
//				if(sensor_function_start_smoke > 6 || Designated_smoke_sensor_upload)
//				{
//					LOS_TaskDelay(50);
//					send_sensor_register(smoke_register);
//					//����֡�ϳɺ���
//					sensor_data[0] = '1';
//					sensor_data[1] = '0';
//					sensor_data[2] = '\0';
//					data_frame_generate_function(0x02, smoke_sentor_head_id, led_head_id, sensor_data);
//					//data_frame_generate_function(TerminalID, smoke_sentor_head_id, led_head_id, sensor_data);
//					wifi_send_function(staip, remoteip, 13+strlen(sensor_data), 0x22, sensor_frame, &WiFisendQueue);
//					sensor_function_start_smoke = 0;
//					Designated_smoke_sensor_upload = 0;
//					continue;
//				}
				/* -------------------------------------------------------------------------------------------------------------- */

				/* -------------------------------------------------------------------------------------------------------------- */
				/*temperature thread*/
//				if(sensor_function_start_temp > 6 || Designated_temp_sensor_upload)
//				{
//					LOS_TaskDelay(50);
//					uvIntSave = LOS_IntLock();
//					// ������ʪ�Ȳɼ���ʹ��Collect_Data_Sht30����ʱ��Ҫ��ѭ������ʹ��
//					Collect_Data_Sht30(&(Tem_Hum[0]), &(Tem_Hum[1]));
//					printf("Temperature=%d Humidity=%d\r\n", (int) Tem_Hum[0],(int) Tem_Hum[1]);
//					LOS_IntRestore(uvIntSave);
//					LOS_TaskDelay(50);
//					//��Tem_Hum��Tem_sentor_head_idת�����ַ���
//					//sprintf(sensor_data,"%d",Tem_Hum[0]);
//					//                   ����֡�ϳɺ���
//					sensor_data[0] = '2';
//					sensor_data[1] = '0';
//					sensor_data[2] = '\0';
//					data_frame_generate_function(0x01, Tem_sentor_head_id, led_head_id, sensor_data);
//					//data_frame_generate_function(TerminalID, Tem_sentor_head_id, led_head_id, sensor_data);
//					//���÷��ͺ���������sensor_frame
//					wifi_send_function(staip, remoteip, 13+strlen(sensor_data), 0x22, sensor_frame, &WiFisendQueue);
//					sensor_function_start_temp = 0;
//					Designated_temp_sensor_upload = 0;
//					continue;
//				}
				/* -------------------------------------------------------------------------------------------------------------- */
			}
        }
    }
}

static int WiFi_task(void)
{
    int ret = -1;
    UINT32 uwRet = LOS_OK;
    UINT32  handle;
    TSK_INIT_PARAM_S rudy_task_init_param;

    memset (&rudy_task_init_param, 0, sizeof (TSK_INIT_PARAM_S));
    rudy_task_init_param.uwArg = (unsigned int)NULL;
    rudy_task_init_param.usTaskPrio = 1;
    rudy_task_init_param.pcName =(char *) "WiFi_task_entry";
    rudy_task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)WiFi_task_entry;
    rudy_task_init_param.uwStackSize = 0x1000;
    uwRet = LOS_TaskCreate(&handle, &rudy_task_init_param);
    if(LOS_OK == uwRet){
        ret = 0;
    }
    return ret;
}

/*
 * 	Function: Initialize modules such as IO, USART
 * 	Parameter In: None
 * 	Parameter Out: None
 */

void device_init(void)
{
	/* GPIO Initialization */
	rcu_periph_clock_enable(RCU_GPIOA);		// LED_Init
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	/* USART Initialization*/
	uart2_init(9600U);
	uart3_init(115200U);
	uart4_init(115200U);					//Uart4_Init
}

int all_tasks_entry(void)
{
	UINT32 uwRet = LOS_OK;
	/* create wifi task */
	uwRet = WiFi_task();
	if (uwRet != LOS_OK)
	{
		return LOS_NOK;
	}
	return uwRet;
}

void ClearSendQueue(sendQueue *Queue)
{
	uint8_t i;		//������
	for(i=0; i<SendQueueLength; i++)
	{
		Queue->sendBuf[i][0] = '\0';
		Queue->nextStorage = 0;
		Queue->currentSendpos = 0;
		Queue->currentNum = 0;
	}
}

/*
 * 	Function: WIFI Send Function. Before using this function, you need check if WIFI_SEND_STA is 1 or not.
 * 	Parameter In:
 * 		*sourceip: 	source IP
 * 		*targetip: 	target IP
 *		len:		packet length (note: not the data length)
 *		mode:		packet mode
 *		*data:		data need be sended
 * 	Parameter Out: 	None
 */

void wifi_send_function(uint8_t* sourceip, uint8_t* targetip, uint8_t len, uint8_t mode, uint8_t* data, sendQueue *WiFisendQueue)
{
	/*�ж϶����Ƿ�����*/
	if(WiFisendQueue->currentNum >= SendQueueLength)
	{
		printf("Sending Queue is full\r\n");
		return;
	}
	uint8_t *pos = WiFisendQueue->sendBuf[WiFisendQueue->nextStorage];
	uint8_t i, datalen;
	uint8_t sourceip32[4], targetip32[4];			//�����ڱ���ǰ�����ֶε�IP��ַ
	/*���ȸ���Queue�е�ֵ*/
	WiFisendQueue->currentNum++;
	WiFisendQueue->nextStorage++;
	if(WiFisendQueue->nextStorage >= SendQueueLength)	//�ж϶�β�Ƿ����
		WiFisendQueue->nextStorage = 0;
	/*��װ���Ĳ����뷢�Ͷ���*/
	IPcharToIP32(sourceip, sourceip32);
	IPcharToIP32(targetip, targetip32);
	for(i=0; i<4; i++)								//����ip��ַ
		*pos++ = sourceip32[i];
	for(i=0; i<4; i++)
		*pos++ = targetip32[i];
	*pos++ = len;
	*pos++ = mode;
	datalen = strlen(data);							//��ȡ���ݳ���
	memcpy(pos, data, datalen);
	*(pos + datalen) = '\0';
}

/*
 * 	Function: data_frame_analyse_function
 * 	Parameter In:
 *	*payload: char type data
 *	mode: �ж�ģʽ
 *	data[4] ������
 * 	Parameter Out:int type data
 */
static void data_frame_analyse_function(uint8_t *payload, uint8_t* APPmode,uint8_t* APPdata)
{
    uint8_t *pos;
    pos=payload;
    *APPmode = *pos++;
    memcpy(APPdata, pos, strlen(pos));
}

//ͨ�����ڶ�ʱ���Ӷ����ڲɼ�Ƶ��
static void ModifyCollectFre(UINT32 fre)
{
	uint8_t uwRet = 1;
	uwRet = LOS_SwtmrDelete(timer_id);
	if (LOS_OK != uwRet)
	{
		dprintf ("delete Timer failed\n");
	}else
	{
		dprintf ("delete Timer succss\n");
	}
	Led_Sensor_swTimer(fre);
}
/*
 * 	Function: Designated_sensor_analyse
 * 	Parameter In:sensor_id   //����ָ��������id
 * 	Parameter Out: ָ���������ɼ���־λ
 */
static void Designated_sensor_analyse(UINT32 sensor_id)
{
	switch(sensor_id)
	{
	   case Tem_sentor_head_id:
		    Designated_temp_sensor_upload +=1;
            break;
	   case smoke_sentor_head_id:
	   	    Designated_smoke_sensor_upload+=1;
            break;
	   case co_sentor_head_id:
		    Designated_co_sensor_upload+=1;
            break;
	}
}
