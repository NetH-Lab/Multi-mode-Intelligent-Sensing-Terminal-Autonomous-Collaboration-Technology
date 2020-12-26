#ifndef __WIFITASK_H__
#define __WIFITASK_H__

#include "wificommand.h"
#include "router.h"

#define ETH_UDP 0
#define ETH_TCP 1
#define WIFIUP 2
#define WIFIDOWN 3
#define LORA 4

#define WIFI_TX_BUF_SIZE 100

void myStringCopy(char* des, char* source, char flag);
void CreatTCP(char id);
uint8_t WIFIInit(void);
void PeriodTransmit(void);
void TCPtransmitUP(uint8_t* data, uint32_t len);
void TCPtransmitDOWN(uint8_t* data, uint32_t len);
void PeriodTransmitRecv(uint8_t* data, uint8_t LinkID);
void NormalTransmitRecv(uint8_t* data);

#endif

