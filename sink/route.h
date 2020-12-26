#ifndef _ROUTE_H_
#define _ROUTE_H_
#include <stdio.h>

#ifdef _WIN32
#include <winsock.h>
#pragma comment (lib,"wsock32.lib")
#pragma warning(disable:4996)
#elif __linux__
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>
#endif

#include <string.h>

/*-------------------------definition-------------------------*/
#define u8 unsigned char            //short for "unsigned char"
#define SENDBUF_LENGTH 128
#define RECVBUF_LENGTH 128
#define ROUTE_TABLE_ENTRY_MAXNUM 8  //route table length

/*-------------------------return value-------------------------*/
#define RETVAL_ROUTE_FAILED 0               //return value: failed to route message
#define RETVAL_ROUTE_SUCCESSFULLY 1         //return value: successfully route message
#define RETVAL_ROUTE_IGNORE 2               //return value: ignore route message
#define RETVAL_ROUTE_INSERT_FAILED 0        //return value: failed to insert route entry
#define RETVAL_ROUTE_INSERT_SUCCESSFULLY 1  //return value: successfully insert route entry
#define RETVAL_ROUTE_SEARCH_FAILED 0        //return value: failed to find route entry
#define RETVAL_ROUTE_DELETE_FAILED 0        //return value: failed to delete route entry
#define RETVAL_ROUTE_DELETE_SUCCESSFULLY 1  //return value: successfully delete route entry

/*route_entry*/
typedef struct route_entry{
    int socket;
    char ip_string[64];
}route_entry;

/*route table*/
typedef struct route_table{
    int current_entry_num;
    route_entry* table[ROUTE_TABLE_ENTRY_MAXNUM];
}route_table;

/*related operation function*/
int insert_route_entry(route_table* r_table, int socket, char* ip_string);
int search_route_entry(route_table* r_table, char* ip_string);
void update_route_entry(route_table* r_table, char* ip_string, int socket);
int pdu_process(int recv_socket, char* recvbuf, int recvbuf_length, int* send_socket, char* sendbuf);
void IPcharToIP32(char* ip, u8* ip32);
void Int_To_Str(int x,char *Str);
void IP32ToIPchar(u8* ip32, char* ip);
#endif