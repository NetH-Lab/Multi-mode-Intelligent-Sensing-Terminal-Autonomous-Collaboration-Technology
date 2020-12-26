#include "wificommand.h"
#include "usart.h"

char TERMINAL_NAME[9] = "IOT-T002";

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
char wifista_ssid[9] = "SmartIoT";			//路由器SSID号
char wifista_encryption[32] = "wpawpa2_aes";	//wpa/wpa2 aes加密方式
char wifista_password[32] = "12345678"; 	//连接密码

//WIFI AP模式,模块对外的无线参数,可自行修改.
char wifiap_ssid[9] = "IOT-AP01";			//对外SSID号
char wifiap_encryption[32] = "wpawpa2_aes";	//wpa/wpa2 aes加密方式
char wifiap_password[32] = "12345678"; 		//连接密码 

//IP地址管理
char staip[16];
char remoteip[16] = "192.168.137.1";

//TCP UDP管理
char UDPport[6] = "10086";
char TCPport[6] = "10010";


//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零UART3_RX_STA;
//     1,清零UART3_RX_STA;
void atk_8266_at_response(uint8_t mode)
{
	if(UART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		UART3_RX_BUF[UART3_RX_STA&0X7FFF]=0;//添加结束符
		printf("%s",UART3_RX_BUF);	//发送到串口
		if(mode)UART3_RX_STA=0;
	} 
}
//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
uint8_t* atk_8266_check_cmd(uint8_t *str)
{
	
	char *strx=0;
	if(UART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		UART3_RX_BUF[UART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)UART3_RX_BUF,(const char*)str);
	} 
	return (uint8_t*)strx;
}
//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t atk_8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0;
	UART3_RX_STA=0;
	//printf("current cmd: %s",cmd);
	u3_printf(cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			LOS_TaskDelay(10);
			if(UART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))
					break;//得到有效数据 
				UART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} //向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
uint8_t atk_8266_send_data(uint8_t *data,uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0;
	UART3_RX_STA=0;
	u3_printf(data);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			LOS_TaskDelay(10);
			if(UART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))break;//得到有效数据 
				UART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}

//获取ATK-ESP8266模块的AP+STA连接状态
//返回值:0，未连接;1,连接成功
uint8_t atk_8266_apsta_check(void)
{
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
	if(atk_8266_check_cmd("+CIPSTATUS:0")&&
		 atk_8266_check_cmd("+CIPSTATUS:1")&&
		 atk_8266_check_cmd("+CIPSTATUS:2")&&
		 atk_8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}
//获取ATK-ESP8266模块的连接状态
//返回值:0,未连接;1,连接成功.
uint8_t atk_8266_consta_check(void)
{
	uint8_t *p;
	uint8_t res;
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
	p=atk_8266_check_cmd("+CIPSTATUS:"); 
	res=*p;									//得到连接状态	
	return res;
}
//获取Client ip地址
//ipbuf:ip地址输出缓存区
void atk_8266_get_wanip(uint8_t* ipbuf)
{
	uint8_t *p,*p1;
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))//获取WAN IP地址失败
		{
			ipbuf[0]=0;
			return;
		}		
		p=atk_8266_check_cmd("\"");
		p1=(uint8_t*)strstr((const char*)(p+1),"\"");
		*p1=0;
		sprintf((char*)ipbuf,"%s",p+1);	
}
