#ifndef __WIFICOMMAND_H__
#define __WIFICOMMAND_H__

#include "systick.h"

#define ESP8266_DATA_HEAD_IDX 10
#define ESP8266_LINKID_IDX 5
#define SourceIP_IDX 0
#define DesIP_IDX 4
#define DataLen_IDX 8
#define MODE_IDX 9
#define DATAHEAD_IDX 10

extern char TERMINAL_NAME[9];

extern char wifista_ssid[9];			//·����SSID��
extern char wifista_encryption[32];	//wpa/wpa2 aes���ܷ�ʽ
extern char wifista_password[32]; 	//��������

//WIFI APģʽ,ģ���������߲���,�������޸�.
extern char wifiap_ssid[9];			//����SSID��
extern char wifiap_encryption[32];	//wpa/wpa2 aes���ܷ�ʽ
extern char wifiap_password[32]; 		//�������� 
//IP
extern char staip[16];
extern char remoteip[16];
//UDP TCP
extern char UDPport[6];
extern char TCPport[6];

void atk_8266_at_response(uint8_t mode);
uint8_t* atk_8266_check_cmd(uint8_t *str);
uint8_t atk_8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime);
uint8_t atk_8266_send_data(uint8_t *data,uint8_t *ack,uint16_t waittime);
uint8_t atk_8266_quit_trans(void);
uint8_t atk_8266_consta_check(void);
void atk_8266_get_wanip(uint8_t* ipbuf);
#endif

