#include "resource.h"

terminal_resource_table* global_resource_strcut;    /*global fusion structure*/

void resource_init()
{
  
    /*1、收发控制接口函数调用
    /*2、解析收到的数据
    data_frame_analyse_function();
    /*3、形成协同资源池
    if(search_ip_entry)
    /*更新资源的数据
    else
    /*插入终端资源
    insert_resource_entry
    /*4、任务驱动下，主动要求终端采集数据，或者是直接从资源池去调用数据，根据场景映射关系对数据进行融合，
    并将结果传送至协同控制模块。
    /*5、协同控制模块，根据融合结果，根据某种优化算法，调用资源池中资源，封装数据，发送给终端，达到M2M的效果。
    /*6、收发控制接口函数调用*/
}

/*  
    Function: insert resource entry to resource table
    Parameter:  r_table:    global resource table
                terminalID: ID which correspond to ip_string
                sensor_type:The type of sensor 
                ip_string:  ip (in string format)
                data :data (in string format)
    Return:     RETVAL_RESOURCE_INSERT_SUCCESSFULLY: successfullt insert
                RETVAL_RESOURCE_INSERT_FAILED: failed to insert
*/
int insert_resource_entry(terminal_resource_table *r_table, uint8_t terminalID, uint8_t sensor_type, uint8_t executor_type, char* ip_string, char* data)
{
    if (r_table->current_entry_num == RESOURCE_TABLE_ENTRY_MAXNUM - 1){
        return RETVAL_RESOURCE_INSERT_FAILED;
    }

    /*make resource entry*/
    terminal_resource* r_entry = (terminal_resource*)malloc(sizeof(terminal_resource));
    if(r_entry == NULL){
        fprintf(stdout, "Memory warning: Can't allocate memory when insert_resource_entry!\n");
        return RETVAL_RESOURCE_INSERT_FAILED;
    }

    /*set terminal ID*/
    r_entry->termID = terminalID;
    fprintf(stdout, "Debug: insert terminalID\n");

    /*set terminal sensor type*/
    r_entry->sen_type = sensor_type;
    fprintf(stdout, "Debug: insert sensor_type\n");

    /*set terminal executor type*/
    r_entry->execute_type = executor_type;
    fprintf(stdout, "Debug: insert executor_type\n");

    /*set terminal ip*/
    memset(r_entry->ip_string, 0, sizeof(r_entry->ip_string));
    strcpy(r_entry->ip_string, ip_string);
    fprintf(stdout, "Debug: insert ip_string\n");

    /*set data of terminal*/
    memset(r_entry->data, 0, sizeof(r_entry->data));
    strcpy(r_entry->data, data);
    fprintf(stdout, "debug: insert r_entry->data: %s\n", r_entry->data); //数据是字符数组，打印有问题

    /*insert resource entry*/
    for(int i = 0; i<RESOURCE_TABLE_ENTRY_MAXNUM; i++){
        if (r_table->table[i]== NULL){
            r_table->table[i] = r_entry;
            break;
        }
    }
    r_table->current_entry_num += 1;
    return RETVAL_RESOURCE_INSERT_SUCCESSFULLY;
}

/*  
    Function: check whether corresponding termID is in the global resource table
    Parameter:  r_table:    global resource table
                termID:     id (in string format)
    Return:     RETVAL_ID_SEARCH_FAILED: failed to find resource entry
                RETVAL_ID_SEARCH_SUCCESSFULLY:   successfully find resource entry
*/
int search_id_entry(terminal_resource_table *r_table, unsigned char termID)
{
    for(int i=0; i<r_table->current_entry_num; i++){
        if(r_table->table[i]->termID == termID)
            return RETVAL_ID_SEARCH_SUCCESSFULLY;
    }
    return RETVAL_ID_SEARCH_FAILED;
}

/*  
    Function: search the ip_string which maps to the input execute_type in the global resource table
    Parameter:  
                r_table: global resource table
                entry_index: store index of executor id
                execute_type:  destination executor type
    Return:     RETVAL_RESOURCE_SEARCH_IP_ENTRY_FAILED:          failed to find ip_string
                RETVAL_RESOURCE_SEARCH_IP_ENTRY_SUCCESSFULLY:   successfully find, return ip_string value
*/
int search_ip_entry(terminal_resource_table *r_table, int* entry_index, int executor_type)
{
    for(int i=0; i<r_table->current_entry_num; i++){
        if(r_table->table[i]->execute_type == executor_type){
            *entry_index = i;
            return RETVAL_RESOURCE_SEARCH_IP_ENTRY_SUCCESSFULLY;
        }
    }

    return RETVAL_RESOURCE_SEARCH_IP_ENTRY_FAILED;          
}

/*  
    Function: update resource data entry
    Parameter in:  r_table:    global resource table
                   term_id:    terminal ID               
    Return:        RETVAL_RESOURCE_DATA_UPDATE_SUCCESSFULLY: successfullt update
                   RETVAL_RESOURCE_DATA_UPDATE_FAILED: failed to update
*/
int update_resource_data_entry(terminal_resource_table* r_table, uint8_t term_id , uint8_t *receive_data)
{
    //according to term_id update data
    for (int i = 0; i < r_table->current_entry_num; i++){
        if (r_table->table[i]->termID == term_id){
            memset(r_table->table[i]->data, 0, sizeof(r_table->table[i]->data));
            strcpy(r_table->table[i]->data, receive_data);
            return RETVAL_RESOURCE_DATA_UPDATE_SUCCESSFULLY;
        }
    }

    return RETVAL_RESOURCE_DATA_UPDATE_SUCCESSFULLY;
}

/*  
    Function:   (1)check whether data in global resource table is enough to do fusion
                (2)if all data are enough, calculate the mean of these data
    Parameter:  
                r_table:        global resource table
                sensor_comb:    specific sensor combination
    Return:     RETVAL_RESOURCE_CHECK_FUSION_CONDITION_FAILED: can't do fusion yet
                RETVAL_RESOURCE_CHECK_FUSION_CONDITION_SUCCESSFULLY: enough data to do fusion
*/
int process_fusion_condition(terminal_resource_table* r_table, sensor_type_combination* sensor_comb)
{
    for(int i=0; i<sensor_comb->sensor_type_num; i++){
        float temp_sum = 0.0f;      /*sum of all data*/
        int is_found = 0;      /*a flag to represent whether this specific type is found in global resouce table*/
        int sensor_num = 0;         /*num of this specific sensor type*/
        
        /*search in global resource table*/
        for(int j=0; j<r_table->current_entry_num; j++){
            if(r_table->table[j]->sen_type == sensor_comb->sensor_type[i]){
                is_found = 1;
                temp_sum += (float)char_to_int(r_table->table[j]->data);
                sensor_num ++;
            }
        }

        if(is_found){
            sensor_comb->sensor_mean_data[i] = temp_sum/sensor_num;
        }else{
            return RETVAL_RESOURCE_CHECK_FUSION_CONDITION_FAILED;
        }
    }

    return RETVAL_RESOURCE_CHECK_FUSION_CONDITION_SUCCESSFULLY;
}

/*  
    Function: search the sensor data which maps to the input sensor_type in the global resource table
    Parameter:  r_table:    global resource table
                sensor_type:  ip (in unsigned char format)
    Return:     0:          failed to find sensor_data_string
                non-zero:   successfully find, return sensor_data value
*/
uint8_t* search_sensor_data_entry(terminal_resource_table *r_table, uint8_t sensor_type)
{
    uint8_t i;
    uint8_t *sensor_data = NULL;
    for (i = 0; i < r_table->current_entry_num; i++)
    {
        if (r_table->table[i]->sen_type == sensor_type)
        {
            sensor_data = r_table->table[i]->data;
            break;
        }
    }
    return sensor_data;
}
