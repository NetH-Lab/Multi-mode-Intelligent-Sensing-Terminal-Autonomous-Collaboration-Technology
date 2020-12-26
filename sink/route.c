#include "route.h"
#include "fusion.h"
#include "resource.h"
#include "pdu.h"

/*--------------------Config Parameters--------------------*/
extern char CLOUD_IP[16];    		          /*IP address of cloud platform*/
extern char AP_IP[16];		                  /*IP address of wifi ap*/
extern char ETH_IF[16];                       /*Name of the Ethernet Port (to internet)*/
extern char ETH_IP[16];                       /*IP address of eth*/
extern unsigned short SINK_STA_TCP_PORT;	  /*TCP port of sink <-> STA communication*/
extern int MAX_STA_NUM;	                      /*max number of STAs*/

/*-----------------------global buffer-----------------------*/
route_table* global_route_table;    /*global route table*/
extern terminal_resource_table* global_resource_strcut;     /*global resource structure*/

/*  
    Function: insert route entry to route table
    Parameter:  r_table:    global route table
                socket:     socket which correspond to ip_string
                ip_string:  ip (in string format)
    Return:     RETVAL_ROUTE_INSERT_SUCCESSFULLY: successfully insert
                RETVAL_ROUTE_INSERT_FAILED: failed to insert
*/
int insert_route_entry(route_table* r_table, int socket, char* ip_string)
{
    if(r_table->current_entry_num == ROUTE_TABLE_ENTRY_MAXNUM-1)
        return RETVAL_ROUTE_INSERT_FAILED;
    
    //make route entry
    route_entry* r_entry = (route_entry*)malloc(sizeof(route_entry));
    if(r_entry == NULL){
        fprintf(stderr, "Memory warning: Can't allocate memory when insert_route_entry!\n");
        return RETVAL_ROUTE_INSERT_FAILED;
    }
    memset(r_entry->ip_string, 0, sizeof(r_entry->ip_string));
    strcpy(r_entry->ip_string, ip_string);
    r_entry->socket = socket;

    //insert route entry
    for(int i=0; i<ROUTE_TABLE_ENTRY_MAXNUM; i++){
        if(r_table->table[i] == NULL){
            r_table->table[i] = r_entry;
            break;
        }
    }
    r_table->current_entry_num += 1;
    return RETVAL_ROUTE_INSERT_SUCCESSFULLY;
}

/*  
    Function: search the socket which maps to the input ip_string in the global route table
    Parameter:  r_table:    global route table
                ip_string:  ip (in string format)
    Return:     0:          failed to find socket
                non-zero:   successfully find, return socket value
*/
int search_route_entry(route_table* r_table, char* ip_string)
{
    int socket = RETVAL_ROUTE_SEARCH_FAILED;
    for(int i=0; i<r_table->current_entry_num; i++){
        if(!strcmp(r_table->table[i]->ip_string, ip_string)){
            socket = r_table->table[i]->socket;
            break;
        }
    }
    return socket;
}

/*  
    Function: update global route table
    Parameter:  r_table:    global route table
                ip_string:  ip (in string format)
                socket:
    Return:     none
*/
void update_route_entry(route_table* r_table, char* ip_string, int socket)
{
    for(int i=0; i<r_table->current_entry_num; i++){
        if(!strcmp(r_table->table[i]->ip_string, ip_string)){
            r_table->table[i]->socket = socket;
            break;
        }
    }
}

/*  
    Function: delete route entry in route table according to socket index
    Parameter:  r_table: global route table
                socket:
    Return:     RETVAL_ROUTE_DELETE_FAILED: failed to delete route table entry
                RETVAL_ROUTE_DELETE_SUCCESSFULLY: successfully delete route table entry
*/
int delete_route_entry(route_table* r_table, int socket)
{
    for(int i=0; i<r_table->current_entry_num; i++){
        if(r_table->table[i]->socket == socket){
            route_entry* temp_pointer = r_table->table[i]; 
            r_table->table[i] = NULL;
            free(temp_pointer);
            return RETVAL_ROUTE_DELETE_SUCCESSFULLY;
        }
    }

    return RETVAL_ROUTE_DELETE_FAILED;
}

/*  
    Function: process received pdu [ !! CORE FUNCTION !! ]
    Parameter:  recv_socket:    receive socket index
                recvbuf:        pointer to receive buffer
                recvbuf_length: length of receive buffer
                send_socket:    send socket index
                sendbuf:        pointer to send buffer
    Return:     RETVAL_ROUTE_FAILED: failed to route
                RETVAL_ROUTE_SUCCESSFULLY: successfully routed
                RETVAL_ROUTE_IGNORE: ignore route message
*/
int pdu_process(int recv_socket, char* recvbuf, int recvbuf_length, int* send_socket, char* sendbuf)
{
    //-----------------I. parse pdu-----------------
    /*initialize pdu*/
    pdu_format pf;
    if(parse_pdu(&pf, recvbuf) == RETVAL_PDU_FORMAT_INVALID){
        fprintf(stderr, "PDU parse warning: invalid pdu format!\n");
        return RETVAL_ROUTE_IGNORE;
    }
    
    //-----------------II. update route table (if necessary)-----------------
    if(RETVAL_ROUTE_SEARCH_FAILED == search_route_entry(global_route_table, pf.source_ip)){
        if(RETVAL_ROUTE_INSERT_FAILED == insert_route_entry(global_route_table, recv_socket, pf.source_ip))
            fprintf(stderr, "Route table warning: route table is full! can't update!\n");
    }else{
        update_route_entry(global_route_table, pf.source_ip, recv_socket);
    }

    //-----------------III. route pdu-----------------
    /*case 1: check whether the pdu is a periodically upload pdu*/
    if(pf.type == PDU_TYPE_PERIODICALLY){
        memset(sendbuf, 0, SENDBUF_LENGTH);
        char payload_buf[64] = {0};
        strcpy(payload_buf, "100");
        make_sendbuf(sendbuf, AP_IP, pf.source_ip, strlen(payload_buf)+PDU_HEADER_LENGTH, PDU_TYPE_PERIODICALLY, payload_buf, strlen(payload_buf)+1);
        *send_socket = recv_socket;
        free(pf.payload);
        return RETVAL_ROUTE_SUCCESSFULLY;
    }

    /*case 2: check whether the pdu is sent to the terminal itself*/
    else if((!strcmp(pf.source_ip, pf.des_ip)) && (pf.type == PDU_TYPE_M2M)){
        memset(sendbuf, 0, SENDBUF_LENGTH);
        strcpy(sendbuf, recvbuf);
        *send_socket = recv_socket;
        free(pf.payload);
        return RETVAL_ROUTE_SUCCESSFULLY;
    }
    
    /*case 3: check whether the pdu is sent to the cloud platform*/
        /*...*/
    
    /*case 4: check whether the pdu is sent to the sink*/
    else if((!strcmp(AP_IP, pf.des_ip)) && (pf.type == PDU_TYPE_APPLICATION)){
        /*1. 解析payload*/
        resource_pdu_format rpf;
        if(RETVAL_RESOURCE_PDU_FORMAT_INVALID == parse_resource_pdu(&rpf, pf.payload)){
            fprintf(stderr, "Resource table warning: invalid resource pdu format!\n");\
            return RETVAL_ROUTE_IGNORE;
        }
        fprintf(stdout, "Debug: finish parse application payload\n");

        /*2. 检查资源表中是否有相应ID，如果有则更新，无则创建&插入*/
        if(RETVAL_ID_SEARCH_FAILED == search_id_entry(global_resource_strcut, rpf.termID)){
            fprintf(stdout, "Debug: id no found\n");
            if(RETVAL_RESOURCE_INSERT_FAILED == insert_resource_entry(global_resource_strcut, rpf.termID, rpf.sen_type, rpf.execute_type, pf.source_ip, rpf.resource_payload))
                fprintf(stderr, "Resource table warning: resource table is full! can't update!\n");
            //return RETVAL_ROUTE_IGNORE;
        }else{
            if(RETVAL_RESOURCE_DATA_UPDATE_FAILED == update_resource_data_entry(global_resource_strcut, rpf.termID, rpf.resource_payload)){
                fprintf(stderr, "Resource table warning: can't update data for Terminal %d!\n", rpf.termID);
                return RETVAL_ROUTE_IGNORE;
            }
        }
        fprintf(stdout, "Debug: finish update resource table\n");

        /*3. 检查资源表中数据是否到达融合要求，若符合，则触发融合算法。如不符合，就跳过*/   
        /*配置传感器组合，用于融合前查询 & 融合前均值计算*/
        sensor_type_combination fire_sensor_comb;
        fire_sensor_comb.sensor_type_num = 3;
        fire_sensor_comb.sensor_type[0] = TEM_SENSOR_TYPE;
        fire_sensor_comb.sensor_type[1] = SMOKE_SENSOR_TYPE;
        fire_sensor_comb.sensor_type[2] = CO_SENSOR_TYPE;

        if(RETVAL_RESOURCE_CHECK_FUSION_CONDITION_FAILED == process_fusion_condition(global_resource_strcut, &fire_sensor_comb)){
            fprintf(stdout, "Fusion module info: can't start fusion, wait for more data!\n");
            return RETVAL_ROUTE_IGNORE;
        }

        /*数据已经满足融合需求，开始融合*/
        fprintf(stdout, "Debug: enter fusion\n");
        float fusion_result = data_fusion(fire_sensor_comb.sensor_mean_data);

        /*融合后开始协同控制*/
        fprintf(stdout, "Debug: enter cooperative_control\n");
        int cooperative_result = cooperative_control(fusion_result);
        fprintf(stdout, "Debug: cooperative_result: %d\n", cooperative_result);

        /*寻找目标执行器*/
        fprintf(stdout, "Debug: enter searching executor\n");
        int executor_index = 0;
        if(RETVAL_RESOURCE_SEARCH_IP_ENTRY_FAILED == search_ip_entry(global_resource_strcut, &executor_index ,LED_EXECUTE_TYPE)){
            fprintf(stdout, "Fusion module info: can't find destination executor, executor id: %d!\n", LED_EXECUTE_TYPE);
            return RETVAL_ROUTE_IGNORE;
        }

        /*map socket*/
        fprintf(stdout, "Debug: enter searching route entry\n");
        *send_socket = search_route_entry(global_route_table, global_resource_strcut->table[executor_index]->ip_string);

        /*make sendbuf*/
        fprintf(stdout, "Debug: enter making sendbuf\n");
        memset(sendbuf, 0, SENDBUF_LENGTH);
        char payload_buf[64] = {0};
        payload_buf[0] = 0x01; /*temp!!*/
        sprintf(payload_buf+sizeof(char)*1, "%d", cooperative_result);
        make_sendbuf(sendbuf, AP_IP, global_resource_strcut->table[executor_index]->ip_string, strlen(payload_buf)+PDU_HEADER_LENGTH, PDU_TYPE_APPLICATION, payload_buf, strlen(payload_buf)+1);

        free(pf.payload);
        return RETVAL_ROUTE_SUCCESSFULLY;
    }

    /*case 5: check whether the pdu is signed as route to other terminals (M2M PDU)*/
    else if(pf.type == PDU_TYPE_M2M){
        int temp_send_socket = search_route_entry(global_route_table, pf.des_ip);
        if(temp_send_socket != 0){
            *send_socket = temp_send_socket;
            strcpy(sendbuf, recvbuf);
            free(pf.payload);
            return RETVAL_ROUTE_SUCCESSFULLY;
        }
        else{
            fprintf(stderr, "Route table warning: unknown destination address! failed to route\n");
            free(pf.payload);
            return RETVAL_ROUTE_FAILED;
        }
    }
}