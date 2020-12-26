#ifndef __RESOURCE_h_
#define __RESOURCE_h_
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "pdu.h"

typedef unsigned char uint8_t;
#define RESOURCE_TABLE_ENTRY_MAXNUM 10

/*-------------------------return value-------------------------*/
#define RETVAL_RESOURCE_INSERT_FAILED 0
#define RETVAL_RESOURCE_INSERT_SUCCESSFULLY 1
#define RETVAL_RESOURCE_CHECK_FUSION_CONDITION_FAILED 0
#define RETVAL_RESOURCE_CHECK_FUSION_CONDITION_SUCCESSFULLY 1
#define RETVAL_RESOURCE_SEARCH_IP_ENTRY_FAILED 0
#define RETVAL_RESOURCE_SEARCH_IP_ENTRY_SUCCESSFULLY 1
#define RETVAL_ID_SEARCH_FAILED 0
#define RETVAL_ID_SEARCH_SUCCESSFULLY 1
#define RETVAL_IP_SEARCH_FAILED 0
#define RETVAL_RESOURCE_DATA_UPDATE_FAILED 0
#define RETVAL_RESOURCE_DATA_UPDATE_SUCCESSFULLY 1

/*-------------------------sensor type-------------------------*/
#define SENSOR_TYPE_NUM 3
#define TEM_SENSOR_TYPE 0x01
#define SMOKE_SENSOR_TYPE  0x02
#define CO_SENSOR_TYPE 0x03

/*-------------------------executor type-------------------------*/
#define EXECUTE_TYPE_NUM 1
#define LED_EXECUTE_TYPE 0x01

/*sensor type combination (for fusion)*/
typedef struct sensor_type_combination{
    int sensor_type_num;
    int sensor_type[SENSOR_TYPE_NUM];
    float sensor_mean_data[SENSOR_TYPE_NUM];
}sensor_type_combination;

/*resource_entry*/
typedef struct resource_entry{
    unsigned char termID;
    unsigned char sen_type;
    unsigned char execute_type;
    char data[50];
    char ip_string[64];
}terminal_resource;

/*resource table*/
typedef struct resource_table{
    int current_entry_num;
    terminal_resource* table[RESOURCE_TABLE_ENTRY_MAXNUM];
}terminal_resource_table;
/*****************/
int insert_resource_entry(terminal_resource_table *r_table, uint8_t terminalID, uint8_t sensor_type, uint8_t executor_type, char* ip_string, char *data);
int update_resource_data_entry(terminal_resource_table* r_table, uint8_t term_id , uint8_t *receive_data);
int process_fusion_condition(terminal_resource_table* r_table, sensor_type_combination* sensor_comb);
int search_id_entry(terminal_resource_table *r_table, unsigned char termID);
int search_ip_entry(terminal_resource_table *r_table, int* entry_index, int executor_type);

#endif