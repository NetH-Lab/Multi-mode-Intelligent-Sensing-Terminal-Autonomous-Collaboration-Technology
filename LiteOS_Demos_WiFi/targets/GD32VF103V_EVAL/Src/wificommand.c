#include "wificommand.h"
#include "usart.h"

char TERMINAL_NAME[9] = "IOT-T002";

//WIFI STAģʽ,����Ҫȥ���ӵ�·�������߲���,��������Լ���·��������,�����޸�.
char wifista_ssid[9] = "SmartIoT";			//·����SSID��
char wifista_encryption[32] = "wpawpa2_aes";	//wpa/wpa2 aes���ܷ�ʽ
char wifista_password[32] = "12345678"; 	//��������

//WIFI APģʽ,ģ���������߲���,�������޸�.
char wifiap_ssid[9] = "IOT-AP01";			//����SSID��
char wifiap_encryption[32] = "wpawpa2_aes";	//wpa/wpa2 aes���ܷ�ʽ
char wifiap_password[32] = "12345678"; 		//�������� 

//IP��ַ����
char staip[16];
char remoteip[16] = "192.168.137.1";

//TCP UDP����
char UDPport[6] = "10086";
char TCPport[6] = "10010";


//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������UART3_RX_STA;
//     1,����UART3_RX_STA;
void atk_8266_at_response(uint8_t mode)
{
	if(UART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		UART3_RX_BUF[UART3_RX_STA&0X7FFF]=0;//��ӽ�����
		printf("%s",UART3_RX_BUF);	//���͵�����
		if(mode)UART3_RX_STA=0;
	} 
}
//ATK-ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
uint8_t* atk_8266_check_cmd(uint8_t *str)
{
	
	char *strx=0;
	if(UART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		UART3_RX_BUF[UART3_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)UART3_RX_BUF,(const char*)str);
	} 
	return (uint8_t*)strx;
}
//��ATK-ESP8266��������
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
uint8_t atk_8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0;
	UART3_RX_STA=0;
	//printf("current cmd: %s",cmd);
	u3_printf(cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			LOS_TaskDelay(10);
			if(UART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_8266_check_cmd(ack))
					break;//�õ���Ч���� 
				UART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} //��ATK-ESP8266����ָ������
//data:���͵�����(����Ҫ��ӻس���)
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)luojian
uint8_t atk_8266_send_data(uint8_t *data,uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0;
	UART3_RX_STA=0;
	u3_printf(data);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			LOS_TaskDelay(10);
			if(UART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_8266_check_cmd(ack))break;//�õ���Ч���� 
				UART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}

//��ȡATK-ESP8266ģ���AP+STA����״̬
//����ֵ:0��δ����;1,���ӳɹ�
uint8_t atk_8266_apsta_check(void)
{
	if(atk_8266_quit_trans())return 0;			//�˳�͸�� 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//����AT+CIPSTATUSָ��,��ѯ����״̬
	if(atk_8266_check_cmd("+CIPSTATUS:0")&&
		 atk_8266_check_cmd("+CIPSTATUS:1")&&
		 atk_8266_check_cmd("+CIPSTATUS:2")&&
		 atk_8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}
//��ȡATK-ESP8266ģ�������״̬
//����ֵ:0,δ����;1,���ӳɹ�.
uint8_t atk_8266_consta_check(void)
{
	uint8_t *p;
	uint8_t res;
	if(atk_8266_quit_trans())return 0;			//�˳�͸�� 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//����AT+CIPSTATUSָ��,��ѯ����״̬
	p=atk_8266_check_cmd("+CIPSTATUS:"); 
	res=*p;									//�õ�����״̬	
	return res;
}
//��ȡClient ip��ַ
//ipbuf:ip��ַ���������
void atk_8266_get_wanip(uint8_t* ipbuf)
{
	uint8_t *p,*p1;
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))//��ȡWAN IP��ַʧ��
		{
			ipbuf[0]=0;
			return;
		}		
		p=atk_8266_check_cmd("\"");
		p1=(uint8_t*)strstr((const char*)(p+1),"\"");
		*p1=0;
		sprintf((char*)ipbuf,"%s",p+1);	
}
