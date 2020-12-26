#ifndef _PDU_H_
#define _PDU_H_
#include <stdio.h>
#include "route.h"

/*-------------------------definition-------------------------*/
#define RETVAL_PDU_FORMAT_VALID 0               //return value: valid pdu format
#define RETVAL_PDU_FORMAT_INVALID 1             //return value: invalid pdu format
#define RETVAL_PDU_FORMAT_ERROR 2               //return value: error occurs when parsing pdu
#define RETVAL_RESOURCE_PDU_FORMAT_VALID 0      //return value: valid resource pdu format
#define RETVAL_RESOURCE_PDU_FORMAT_INVALID 1    //return value: invalid resource pdu format
#define RETVAL_RESOURCE_PDU_FORMAT_ERROR 2      //return value: error occurs when parsing pdu

#define RETVAL_MAKE_SENDBUF_SUCCESSFULLY 0  //return value: successfully make sendbuf
#define RETVAL_MAKE_SENDBUF_FAILED 1        //return value: failed to make sendbuf

/*-------------------------FIELD LENGTH-------------------------*/
#define PDU_HEADER_LENGTH 10                //length of pdu header (sip[4 bytes], dip[4 bytes], length[1 byte], type[1 byte])
#define RESOURCE_PAYLOAD_BUF_SIZE 8         //length of resource pdu payload
/*-------------------------pdu type-------------------------*/
#define PDU_TYPE_PERIODICALLY 0         //message type: periodically upload message
#define PDU_TYPE_M2M 1                  //message type: M2M message
#define PDU_TYPE_APPLICATION 2          //message type: need to be processes by the application runs on sink


/*pdu format*/
typedef struct pdu_format{
    unsigned char source_ip_num[4];     /*source ip (unsigned int format)*/
    unsigned char des_ip_num[4];        /*destination ip (unsigned int format)*/
    char source_ip[64];                 /*source ip (string format)*/
    char des_ip[64];                    /*destination ip (string format)*/
    unsigned char length;               /*pdu length*/
    unsigned char type;                 /*pdu type*/
    char* payload;                      /*pointer to payload buffer*/
}pdu_format;

/*resource pdu format*/
typedef struct resource_pdu_format{
    unsigned char termID;               /*terminal ID*/
    unsigned char sen_type;             /*sensor type*/
    unsigned char execute_type;         /*executor type*/
    char* resource_payload;             /*application payload*/
}resource_pdu_format;

int parse_pdu(pdu_format* pf, char* recvbuf);
int make_sendbuf(char* sendbuf, char* source_ip, char* des_ip, char length, char type, char* payload_buf, int payload_buf_length);
int parse_resource_pdu(resource_pdu_format* rpf, char* payload);

/*-------------------------Operation Function-------------------------*/
int char_to_int(uint8_t *data);
void Int_To_Str(int x,char *Str);
void IP32ToIPchar(u8* ip32, char* ip);
void IPcharToIP32(char* ip, u8* ip32);

#endif