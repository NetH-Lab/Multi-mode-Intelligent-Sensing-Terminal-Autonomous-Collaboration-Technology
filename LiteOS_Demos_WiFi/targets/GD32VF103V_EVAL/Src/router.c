#include "router.h"

route RouteList[routeLenge];

void InitRouteList(void)
{
	uint8_t i;
	for(i=0; i<routeLenge; i++)
		RouteList[i].lifetime = 0;
}

// ����·�ɱ��1�ɹ���0ʧ��
uint8_t CreatRouteEntry(char* des, char* nextHop, char LinkID)
{
	uint8_t i;
	for(i=0; i<routeLenge; i++)			//Ѱ��һ����·�ɱ�
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

//��ѯ·�ɱ������·�ɱ�����
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

//IP�ַ���תΪ4�ֽ�uint8_t�����������'\0'����
void IPcharToIP32(char* ip, uint8_t* ip32)
{
	uint8_t i, j, ipInt, idx = 0;
	uint8_t ipIndex = 0;
	char temp[4];
	for(i=0; i<=strlen(ip); i++)
	{
		if(ip[i] == '.' || ip[i] == '\0')
		{
			for(j=0; ipIndex<i; j++)		//����8λ��IP
			{
				temp[j] = ip[ipIndex];
				ipIndex++;
			}
			temp[j] = '\0';
			ipIndex++;					//ipIndexָ����һ��8λIP
			ipInt = atoi(temp);	//��char��ת����8λ��IP
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

//IP4�ֽ�charתΪ�ַ���
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




