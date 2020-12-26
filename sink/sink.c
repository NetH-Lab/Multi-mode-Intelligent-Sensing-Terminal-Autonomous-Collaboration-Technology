/*--------------------------------------------------------------------------------
	sink: Establishing multiple connections with multiple STAs, routing message
       among multiple STAs.
	Author: UESTC KB109, Zhuobin Huang, zobin1999@gmail.com
	Update: 12/17 2020
    Enviroment: Can compile & run under Windows & Linux
--------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include "configure.h"
#include "SockList.h"
#include "route.h"
#include "fusion.h"
#include "resource.h"

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
#include <sys/time.h> //to use struct timeval
#include <fcntl.h> //to use fcntl()
#endif

/*--------------------Config Parameters--------------------*/
extern char CLOUD_IP[16];    		          /*IP address of cloud platform*/
extern char AP_IP[16];		                  /*IP address of wifi ap*/
extern char ETH_IF[16];                       /*Name of the Ethernet Port (to internet)*/
extern char ETH_IP[16];                       /*IP address of eth*/
extern unsigned short SINK_STA_TCP_PORT;	  /*TCP port of sink <-> STA communication*/
extern int MAX_STA_NUM;	                      /*max number of STAs*/

/*-----------------------global buffer-----------------------*/
extern route_table* global_route_table;                     /*global route table*/
extern terminal_resource_table* global_resource_strcut;     /*global resource structure*/

void printf_error();
void delay();

int main()
{
    /*-----------------------initial valuables-----------------------*/
    int retval;                                  /*return value*/
    int s, newsock;                              /*socket id*/
    char recvbuf[RECVBUF_LENGTH] = {0};          /*recv buf*/
    char sendbuf[SENDBUF_LENGTH] = {0};          /*send buf*/
    struct sockaddr_in server_addr, remote_addr; /*socket address*/
    socket_list sock_list;                       /*socket list*/
    fd_set readfds, writefds, exceptfds;         /*file set*/
    struct timeval timeout;                      /*select() timeout configuration*/
    unsigned long arg = 1;                    
    int len;

    /*-----------------------allocate space-----------------------*/
    /*route table*/
    global_route_table = (route_table*)malloc(sizeof(route_table)); 
    if(global_route_table == NULL){
        fprintf(stderr, "Allocate error: can't allocate memory space for route table");
        goto exit;
    }
    
    /*resource table*/
    global_resource_strcut = (terminal_resource_table*)malloc(sizeof(terminal_resource_table));
    if(global_resource_strcut == NULL){
        fprintf(stderr, "Allocate error: can't allocate memory space for resource table\n");
        goto exit;
    }
    memset(global_resource_strcut, 0, sizeof(global_resource_strcut));
    for (int i = 0; i < RESOURCE_TABLE_ENTRY_MAXNUM; i++){
        global_resource_strcut->table[i]=NULL;
    }
    
    /*-----------------------read configure parameters from config file-----------------------*/
    if(config_parameter()){
      printf_config();
    }
    else{
      fprintf(stderr, "Config error: config failed, Exit!\n");
      goto exit;
    }
    
    /*-----------------------load socket DLL under Windows environment-----------------------*/
    #ifdef _WIN32
        WSAData wsa;
        WSAStartup(0x101, &wsa);
    #endif

    /*-----------------------create socket-----------------------*/
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        printf_error();
        goto exit;
    }

    /*-----------------------bind socket to local wlan IP address-----------------------*/
    server_addr.sin_family = AF_INET;
    #ifdef _WIN32
        //server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        server_addr.sin_addr.S_un.S_addr = inet_addr(AP_IP);
    #elif __linux__
        //server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_addr.s_addr = inet_addr(AP_IP);
    #endif
    server_addr.sin_port = htons(SINK_STA_TCP_PORT);
    retval = bind(s, (struct sockaddr*) & server_addr, sizeof(server_addr));
    if (retval < 0) {
        printf_error();
        goto exit;
    }

    /*-----------------------set socket as listen mode-----------------------*/
    retval = listen(s, 5);
    if (retval < 0) {
        printf_error();
        goto exit;
    }

    /*-----------------------config timeout value-----------------------*/
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    /*-----------------------config socket list-----------------------*/
    init_list(&sock_list);
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    sock_list.MainSock = s;

    /*-----------------------set mainsock as non-block mode-----------------------*/
    #ifdef _WIN32
        ioctlsocket(sock_list.MainSock, FIONBIO, &arg);
    #elif __linux__
        int flags = fcntl(sock_list.MainSock, F_GETFL, 0);
        fcntl(sock_list.MainSock, F_SETFL, flags | O_NONBLOCK);
    #endif

    /*-----------------------processing cycle-----------------------*/
    while (1) {
        /*1.make up state list*/
        make_fdlist(&sock_list, &readfds);
        make_fdlist(&sock_list, &writefds);
        make_fdlist(&sock_list, &exceptfds);

        /*2. select*/
        retval = select(FD_SETSIZE, &readfds, &writefds, &exceptfds, &timeout);
        if (retval < 0) {
            printf_error();
            goto exit;
        }

        /*3. check whether MainSock is in the readfds, if so, try to accept the connection*/
        if (FD_ISSET(sock_list.MainSock, &readfds)) {
            int len = sizeof(remote_addr);
            newsock = accept(sock_list.MainSock, (struct sockaddr*)&remote_addr, &len);
            if (newsock < 0)
                continue;
            fprintf(stdout, "\n\n--------------------INFO--------------------\n");
            fprintf(stdout, "Info: accept a connection, from %s : %d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
            fprintf(stdout, "--------------------------------------------\n\n");
            insert_list(newsock, &sock_list);
        }

        /*4. check whether other sockets is in the readfds/writefds/exceptfds, if so, process their read/write/except events*/
        for (int i = 0; i < SOCKETLIST_LENGTH; i++) {
            if (sock_list.sock_array[i] == 0)
                continue;
            newsock = sock_list.sock_array[i];

            /*check readfds*/
            if (FD_ISSET(newsock, &readfds)) {
                /*recv data*/
                memset(recvbuf, '\0', sizeof(recvbuf));
                retval = recv(newsock, recvbuf, sizeof(recvbuf), 0);
                if (retval == 0) {
                #ifdef _WIN32
                        closesocket(newsock);
                #elif __linux__
                        close(newsock);
                #endif
                    delete_list(newsock, &sock_list);
                    fprintf(stderr, "Info: close a socket\n");
                    continue;
                }
                else if (retval == -1) {
                #ifdef _WIN32
                    retval = WSAGetLastError();
                    if (retval == WSAEWOULDBLOCK)//if it is a timeout error
                        continue;
                    closesocket(newsock);
                #elif __linux__
                    if (errno == EAGAIN)//if it is a timeout error
                        continue;
                    close(newsock);
                #endif
                    fprintf(stderr, "Info: close a socket\n");
                    delete_list(newsock, &sock_list);
                    continue;
                }
                
                /*show recv data*/
                recvbuf[retval] = 0;
                fprintf(stdout, "\n\n--------------------PDU--------------------\n");
                fprintf(stdout, "Info: received data: %s\n", recvbuf);

                /*delay is necessary!*/
                delay();
                
		        /*process received packet*/
                int send_sock = 0;
                int pdu_process_state = pdu_process(newsock, recvbuf, retval, &send_sock, sendbuf);
                if(RETVAL_ROUTE_SUCCESSFULLY == pdu_process_state){
                    /*if need to send PDU to underlying device*/
	                if(strlen(sendbuf) != 0){
		                retval = send(send_sock, sendbuf, strlen(sendbuf) + 1, 0);
                    if (retval < 0) {
                      printf_error();
                      goto exit;
                    }
                    fprintf(stdout, "Info: send data: %s, length:%d\n", sendbuf, retval);
                    fprintf(stdout, "-------------------------------------------\n\n");
                  }
                }else if(RETVAL_ROUTE_IGNORE == pdu_process_state){
                    fprintf(stderr, "Info: route module ignore this pdu.\n");
                    fprintf(stdout, "-------------------------------------------\n\n");
                }else{
                    fprintf(stderr, "Warning: route module failed to route this pdu.\n");
                    fprintf(stdout, "-------------------------------------------\n\n");
                }
            }

            /*check writefds*/
            if (FD_ISSET(newsock, &writefds)) {
                /*to be added*/
            }

            /*check exceptfds*/
            if (FD_ISSET(newsock, &exceptfds)) {
                /*to be added*/
            }
        }
        /*5. Clear all fds*/
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);
    }

    exit: {
    #ifdef _WIN32
        if (s >= 0) {
            closesocket(s);
        }
        WSACleanup();
    #elif __linux__
        if (s >= 0) {
            close(s);
        }
    #endif
    }
}

/*  
    Function:   print error info
    Parameter:  none
    Return:     none
*/
void printf_error()
{
#ifdef _WIN32
    int retval = WSAGetLastError();
    fprintf(stderr, "Socket error: %d\n", retval);
#elif __linux__
    fprintf(stderr, "Socket error: %s(errno: %d)\n", strerror(errno), errno);
#endif
}

/*  
    Function:   simple delay function :)
    Parameter:  none
    Return:     none
*/
void delay()
{
    for(int i=0; i<100000; i++)
        for(int j=0; j<1100; j++);
}
