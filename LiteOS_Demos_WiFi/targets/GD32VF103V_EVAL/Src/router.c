#include "router.h"

route RouteList[routeLenge];

void InitRouteList(void)
{
	uint8_t i;
	for(i=0; i<routeLenge; i++)
		RouteList[i].lifetime = 0;
}

// 创建路由表项，1成功，0失败
uint8_t CreatRouteEntry(char* des, char* nextHop, char LinkID)
{
	uint8_t i;
	for(i=0; i<routeLenge; i++)			//寻找一个空路由表
	{
		if(RouteList[i].lifetime == 0)
		{
			memcpy(RouteList[i].desIP, des, 16);
			memcpy(RouteList[i].nextHopIP, nextHop, 16);
			RouteList[i].LinkID = LinkID;
			RouteList[i].lifetime = 10;
			return 1;
		}
	}
	return 0;
}

//查询路由表项，返回路由表项编号
uint8_t FindEntry(char* des)
{
	uint8_t i;
	for(i=0; i<routeLenge; i++)
	{
		if(RouteList[i].lifetime != 0)
		{
			if(strstr(RouteList[i].desIP, des))
				return i;
		}
	}
	return routeLenge;
}

//IP字符串转为4字节uint8_t，输入必须以'\0'结束
void IPcharToIP32(char* ip, uint8_t* ip32)
{
	uint8_t i, j, ipInt, idx = 0;
	uint8_t ipIndex = 0;
	char temp[4];
	for(i=0; i<=strlen(ip); i++)
	{
		if(ip[i] == '.' || ip[i] == '\0')
		{
			for(j=0; ipIndex<i; j++)		//复制8位的IP
			{
				temp[j] = ip[ipIndex];
				ipIndex++;
			}
			temp[j] = '\0';
			ipIndex++;					//ipIndex指向下一组8位IP
			ipInt = atoi(temp);	//将char型转换成8位的IP
			ip32[idx] = ipInt;
			idx++;
		}
	}
}

void Int_To_Str(int x,char *Str)
{
	 int t;
	 char *Ptr,Buf[5];                 //???????????????,
	 int i = 0;                             //????
	 Ptr = Str;                           //??????????,??????,????????(????),
	 if(x < 10)  // ?????10,???0x??
	 {
		 *Ptr ++ = x+0x30;
	 }
	 else
	 {
		 while(x > 0)
		 {
			 t = x % 10;
			 x = x / 10;
			 Buf[i++] = t+0x30;  // ?????????ASCII???
		 }
		 i -- ;
		 for(;i >= 0;i --)  // ?????????
		 {
			*(Ptr++) = Buf[i];
		 }
	 }
	 *Ptr = '\0';
}

//IP4字节char转为字符串
void IP32ToIPchar(uint8_t* ip32, char* ip)
{
	uint8_t i,j;
	char temp[10];
	char* idx = ip;
	for(i=0; i<3; i++)
	{
		Int_To_Str(ip32[i], temp);
		for(j=0; j<strlen(temp); j++)
		{
			*idx = temp[j];
			idx++;
		}
		*idx = '.';
		idx++;
	}
	Int_To_Str(ip32[3], temp);
	for(j=0; j<strlen(temp); j++)
	{
		*idx = temp[j];
		idx++;
	}
	*idx = '\0';
}




