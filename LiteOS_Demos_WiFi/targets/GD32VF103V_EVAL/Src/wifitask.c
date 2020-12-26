#include "wifitask.h"
#include "usart.h"

uint8_t WIFI_RECV_STA = 0;		//���ܱ�־λ��Ϊ1��������Ϣ��Ҫ������
uint8_t WIFI_RECV_BUF[WIFI_TX_BUF_SIZE];

uint16_t PeriodUpdateCounter = 0;	//�����Է��ͼ��������ڶ�ʱ�����ۼ�
uint8_t PeriodUpdateEnable = 0;		//�����Է���ʹ��λ�����ڶ�ʱ��

uint16_t SendCounter = 0;	//���Ϳ��Ƽ�ʱ�������ڵ��ڷ���Ƶ��

void myStringCopy(char* des, char* source, char flag)
{
	while(1)
	{
		if(*source != flag)
		{
			*des = *source;
			des++;
			source++;
		}
		else
		{
			*des = '\0';
			break;
		}
	}
}
/*Function: Find a specific flag in a string. 
	Time means the one which you want to return if the flag repeats in a string*/
uint8_t* myFind(uint8_t* begin, char flag, uint8_t time)
{
	uint8_t temp = 0;
	while(*begin != '\0')
	{
		if(*begin == flag)
			temp++;
		if(temp == time)
			return begin;
		begin++;
	}
	return begin;
}

uint8_t WIFIInit(void)
{
	//��ʼ������
	uint8_t *temp;
	char *str = 0;
	char buf[100];							//����100�ֽ��ڴ�
	char *p_buf = buf;
	atk_8266_send_cmd("AT+RST\r\n","OK",100);			//RST
	atk_8266_send_cmd("AT+CWMODE=1\r\n","OK",100);		//����WIFI STAģʽ
	//���ҿ��õ�SSID
	sprintf((char*)p_buf,"AT+CWLAP\r\n");
	printf("Searching for SSID......\r\n");
	while(1)
	{
		if(!(atk_8266_send_cmd(p_buf,"IOT",500)))		//�������IoT�ڵ���Ա�����
		{
			str=strstr((const char*)UART3_RX_BUF,(const char*)"IOT-SINK");		//�ȼ���Ƿ�������sink�ڵ�
			if(str)
			{
				memcpy(wifista_ssid, str, 8);	//��������
				printf("wifista_ssid:%s\r\n",(uint8_t*)wifista_ssid);
			}
			else
			{
				str=strstr((const char*)UART3_RX_BUF,(const char*)"IOT");	//��������STA����������
				memcpy(wifista_ssid, str, 8);	//��������
				printf("wifista_ssid:%s\r\n",(uint8_t*)wifista_ssid);
			}
			break;
		}
	}
	//���ӵ�Ŀ��AP
	sprintf((char*)p_buf,"AT+CWJAP=\"%s\",\"%s\"\r\n",wifista_ssid,wifista_password);//�������߲���:ssid,����
	atk_8266_send_cmd(p_buf,"OK",1000);						//����Ŀ��·�����������IP
	while(atk_8266_send_cmd("AT+CIFSR\r\n","STAIP",100));   //����Ƿ���STA IP
	//���ñ���IP
	str=strstr((const char*)UART3_RX_BUF,(const char*)"STAIP");
	myStringCopy(staip, str+7, '"');
//	memcpy(staip, str+7, 15);	//��������
	printf("STAIP:%s\r\n",staip);
	//��������ip
	memcpy(remoteip, staip, 16);
	temp = myFind(remoteip, '.', 3);
	if(*temp != '\0')
	{
		*(temp+1) = '1';
		*(temp+2) = '\0';
		printf("REMOTEIP:%s\r\n", remoteip);
	}
	else
		printf("RemoteIP configuration error\r\n");
	
	//���÷�����
	atk_8266_send_cmd("AT+CIPMUX=1\r\n","OK",100);		//����������
	LOS_TaskDelay(500);
	sprintf((char*)p_buf,"AT+CIPSERVER=1,%s\r\n",(uint8_t*)TCPport);
	while(atk_8266_send_cmd(p_buf,"OK",100));     //����Serverģʽ���˿ں�Ϊ10010
	LOS_TaskDelay(500);
	atk_8266_send_cmd("AT+CIPSTO=1200\r\n","OK",100);     //���÷�������ʱʱ��
	//����UDP
	sprintf((char*)p_buf,"AT+CIPSTART=0,\"UDP\",\"192.168.137.100\",8000,8000,2\r\n");
	LOS_TaskDelay(200);
	atk_8266_send_cmd(p_buf,"OK",500);
	LOS_TaskDelay(500);
//	printf("%s\r\n",UART3_RX_BUF);
//	sprintf((char*)p,"ATK-8266%02d\r\n",10086);//��������
//	atk_8266_send_cmd("AT+CIPSEND=10","OK",100);  //����ָ�����ȵ�����
//	printf("%s\r\n",UART3_RX_BUF);
//	LOS_TaskDelay(500);
//	atk_8266_send_data(p,"OK",100);  //����ָ�����ȵ�����
//	printf("%s\r\n",UART3_RX_BUF);
	return 0;
}
void CreatTCP(char id)
{
	uint8_t *p, buf[32];
	p=buf;							//����32�ֽ��ڴ�
	sprintf((char*)p,"AT+CIPSTART=%c,\"TCP\",\"%s\",%s\r\n",id,remoteip,(uint8_t*)TCPport);    //����Ŀ��TCP������
	while(atk_8266_send_cmd(p,"OK",200));
	printf("TCP%c Creat Successfully\r\n", id);
}

//�����Ը��º���
void PeriodTransmit(void)
{
	uint8_t data[16];
	uint8_t sourceIP[4];
	//��ͷ
	IPcharToIP32(staip, sourceIP);
	memcpy(data, sourceIP, 4);
	data[4] = 1;
	data[5] = 1;
	data[6] = 1;
	data[7] = 1;
	data[8] = 15;
	data[9] = 2;
	//���ݲ���
	data[10] = 'H';
	data[11] = 'e';
	data[12] = 'l';
	data[13] = 'l';
	data[14] = 'o';
	data[15] = '\0';
	TCPtransmitUP(data, data[8]);
}

//���д��䣬����Ŀ�Ľڵ��ΪĬ������
void TCPtransmitUP(uint8_t* data, uint32_t len)
{
	uint8_t *p, buf[100];
	p=buf;							//����32�ֽ��ڴ�
	sprintf((char*)p,"AT+CIPSEND=1,%d\r\n", len);
	atk_8266_send_cmd(p,"OK",200);  //����ָ�����ȵ�����
	LOS_TaskDelay(200);
	while(atk_8266_send_data(data,"OK",100));  //����ָ�����ȵ�����
}
//���д��䣬���ѯ·�ɱ�
void TCPtransmitDOWN(uint8_t* data, uint32_t len)
{
	uint8_t des32[4], routerNum, buf[100];
	uint8_t *p = buf;
	char desIP[16];
	memcpy(des32, data+4, 4);
	IP32ToIPchar(des32, desIP);			//�������е�IP��ַת�����ַ���
	routerNum = FindEntry(desIP);		//���Ҷ�Ӧ��·�ɱ���
	if(routerNum != routeLenge)			//�ҵ���·�ɱ�	
	{
		sprintf((char*)p,"AT+CIPSEND=%c,%d\r\n",RouteList[routerNum].LinkID, len);
		atk_8266_send_cmd(p,"OK",200);
		LOS_TaskDelay(200);
		atk_8266_send_data(data,"OK",100);  //����ָ�����ȵ�����
	}
	else
		printf("û���ҵ���Ӧ·�ɱ��Ŀ�ĵ�ַ��%s\r\n", desIP);
}

//���ڻ㱨���ģ�ת��������·�ɱ�
void PeriodTransmitRecv(uint8_t* data, uint8_t LinkID)
{
	char sourceIP[16], desIP[16], nextHop[16];
	uint8_t sourceIP32[4],desIP32[4];
	memcpy(sourceIP32, data+SourceIP_IDX, 4);
	memcpy(desIP32, data+DesIP_IDX, 4);
	IP32ToIPchar(sourceIP32, sourceIP);		//��ȡ��ԴIP��ַ����Ϊ·�ɱ��е�Ŀ��IP
	IP32ToIPchar(desIP32, desIP);
	//��ѯ�ñ��������ĸ�IP������
	while(atk_8266_send_data("AT+CIPSTATUS\r\n", "OK", 100));
	
	
}

//��ͨ���Ĵ���
void NormalTransmitRecv(uint8_t* data)
{
	uint8_t targetip32[4];
}





