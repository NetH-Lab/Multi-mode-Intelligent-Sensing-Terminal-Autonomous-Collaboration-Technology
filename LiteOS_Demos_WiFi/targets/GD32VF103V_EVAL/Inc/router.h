#ifndef __ROUTER_H__
#define __ROUTER_H__

#include "string.h"
#include "stdlib.h"
#include "usart.h"

#define routeLenge 10

typedef struct RouteEntry
{
	uint8_t lifetime;
	char desIP[16];
	char nextHopIP[16];
	char LinkID;
}route;

extern route RouteList[10];

void InitRouteList(void);
uint8_t CreatRouteEntry(char* des, char* nextHop, char LinkID);
uint8_t FindEntry(char* des);
void IPcharToIP32(char* ip, uint8_t* ip32);
void IP32ToIPchar(uint8_t* ip32, char* ip);
#endif

