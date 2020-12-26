#include "wifitask.h"
#include "usart.h"

uint8_t WIFI_RECV_STA = 0;		//接受标志位，为1代表有消息需要被处理
uint8_t WIFI_RECV_BUF[WIFI_TX_BUF_SIZE];

uint16_t PeriodUpdateCounter = 0;	//周期性发送计数器，在定时器中累加
uint8_t PeriodUpdateEnable = 0;		//周期性发送使能位，用于定时器

uint16_t SendCounter = 0;	//发送控制计时器，用于调节发送频率

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
	//初始化变量
	uint8_t *temp;
	char *str = 0;
	char buf[100];							//申请100字节内存
	char *p_buf = buf;
	atk_8266_send_cmd("AT+RST\r\n","OK",100);			//RST
	atk_8266_send_cmd("AT+CWMODE=1\r\n","OK",100);		//设置WIFI STA模式
	//查找可用地SSID
	sprintf((char*)p_buf,"AT+CWLAP\r\n");
	printf("Searching for SSID......\r\n");
	while(1)
	{
		if(!(atk_8266_send_cmd(p_buf,"IOT",500)))		//检测有无IoT节点可以被连接
		{
			str=strstr((const char*)UART3_RX_BUF,(const char*)"IOT-SINK");		//先检查是否能连接sink节点
			if(str)
			{
				memcpy(wifista_ssid, str, 8);	//复制数据
				printf("wifista_ssid:%s\r\n",(uint8_t*)wifista_ssid);
			}
			else
			{
				str=strstr((const char*)UART3_RX_BUF,(const char*)"IOT");	//连接其他STA，多跳传输
				memcpy(wifista_ssid, str, 8);	//复制数据
				printf("wifista_ssid:%s\r\n",(uint8_t*)wifista_ssid);
			}
			break;
		}
	}
	//连接到目标AP
	sprintf((char*)p_buf,"AT+CWJAP=\"%s\",\"%s\"\r\n",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	atk_8266_send_cmd(p_buf,"OK",1000);						//连接目标路由器，并获得IP
	while(atk_8266_send_cmd("AT+CIFSR\r\n","STAIP",100));   //检测是否获得STA IP
	//配置本地IP
	str=strstr((const char*)UART3_RX_BUF,(const char*)"STAIP");
	myStringCopy(staip, str+7, '"');
//	memcpy(staip, str+7, 15);	//复制数据
	printf("STAIP:%s\r\n",staip);
	//配置网关ip
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
	
	//配置服务器
	atk_8266_send_cmd("AT+CIPMUX=1\r\n","OK",100);		//开启多连接
	LOS_TaskDelay(500);
	sprintf((char*)p_buf,"AT+CIPSERVER=1,%s\r\n",(uint8_t*)TCPport);
	while(atk_8266_send_cmd(p_buf,"OK",100));     //开启Server模式，端口号为10010
	LOS_TaskDelay(500);
	atk_8266_send_cmd("AT+CIPSTO=1200\r\n","OK",100);     //设置服务器超时时间
	//配置UDP
	sprintf((char*)p_buf,"AT+CIPSTART=0,\"UDP\",\"192.168.137.100\",8000,8000,2\r\n");
	LOS_TaskDelay(200);
	atk_8266_send_cmd(p_buf,"OK",500);
	LOS_TaskDelay(500);
//	printf("%s\r\n",UART3_RX_BUF);
//	sprintf((char*)p,"ATK-8266%02d\r\n",10086);//测试数据
//	atk_8266_send_cmd("AT+CIPSEND=10","OK",100);  //发送指定长度的数据
//	printf("%s\r\n",UART3_RX_BUF);
//	LOS_TaskDelay(500);
//	atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
//	printf("%s\r\n",UART3_RX_BUF);
	return 0;
}
void CreatTCP(char id)
{
	uint8_t *p, buf[32];
	p=buf;							//申请32字节内存
	sprintf((char*)p,"AT+CIPSTART=%c,\"TCP\",\"%s\",%s\r\n",id,remoteip,(uint8_t*)TCPport);    //配置目标TCP服务器
	while(atk_8266_send_cmd(p,"OK",200));
	printf("TCP%c Creat Successfully\r\n", id);
}

//周期性更新函数
void PeriodTransmit(void)
{
	uint8_t data[16];
	uint8_t sourceIP[4];
	//报头
	IPcharToIP32(staip, sourceIP);
	memcpy(data, sourceIP, 4);
	data[4] = 1;
	data[5] = 1;
	data[6] = 1;
	data[7] = 1;
	data[8] = 15;
	data[9] = 2;
	//数据部分
	data[10] = 'H';
	data[11] = 'e';
	data[12] = 'l';
	data[13] = 'l';
	data[14] = 'o';
	data[15] = '\0';
	TCPtransmitUP(data, data[8]);
}

//上行传输，所有目的节点均为默认网关
void TCPtransmitUP(uint8_t* data, uint32_t len)
{
	uint8_t *p, buf[100];
	p=buf;							//申请32字节内存
	sprintf((char*)p,"AT+CIPSEND=1,%d\r\n", len);
	atk_8266_send_cmd(p,"OK",200);  //发送指定长度的数据
	LOS_TaskDelay(200);
	while(atk_8266_send_data(data,"OK",100));  //发送指定长度的数据
}
//下行传输，需查询路由表
void TCPtransmitDOWN(uint8_t* data, uint32_t len)
{
	uint8_t des32[4], routerNum, buf[100];
	uint8_t *p = buf;
	char desIP[16];
	memcpy(des32, data+4, 4);
	IP32ToIPchar(des32, desIP);			//将报文中的IP地址转换成字符串
	routerNum = FindEntry(desIP);		//查找对应的路由表编号
	if(routerNum != routeLenge)			//找到了路由表	
	{
		sprintf((char*)p,"AT+CIPSEND=%c,%d\r\n",RouteList[routerNum].LinkID, len);
		atk_8266_send_cmd(p,"OK",200);
		LOS_TaskDelay(200);
		atk_8266_send_data(data,"OK",100);  //发送指定长度的数据
	}
	else
		printf("没有找到对应路由表项，目的地址：%s\r\n", desIP);
}

//周期汇报报文，转发并更新路由表
void PeriodTransmitRecv(uint8_t* data, uint8_t LinkID)
{
	char sourceIP[16], desIP[16], nextHop[16];
	uint8_t sourceIP32[4],desIP32[4];
	memcpy(sourceIP32, data+SourceIP_IDX, 4);
	memcpy(desIP32, data+DesIP_IDX, 4);
	IP32ToIPchar(sourceIP32, sourceIP);		//获取到源IP地址，作为路由表中的目的IP
	IP32ToIPchar(desIP32, desIP);
	//查询该报文是由哪个IP发来的
	while(atk_8266_send_data("AT+CIPSTATUS\r\n", "OK", 100));
	
	
}

//普通报文处理
void NormalTransmitRecv(uint8_t* data)
{
	uint8_t targetip32[4];
}





