#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "configure.h"

/*--------------------Config Parameters--------------------*/
char CLOUD_IP[16] = {0};    		          /*IP address of cloud platform*/
char AP_IP[16] = {0};		                  /*IP address of wifi ap*/
char ETH_IP[16] = {0};		                /*IP address of eth*/
char ETH_IF[16] = {0};                    /*Name of the Ethernet Port (to internet)*/
unsigned short SINK_STA_TCP_PORT = 0;	    /*TCP port of sink <-> STA communication*/
int MAX_STA_NUM = 0;	                    /*max number of STAs*/

/*
   Function: read parameter from config file
   Parameter: none
   Return: 0: failed to read in parameters  
           1: successfully read in parameters  
*/
int config_parameter()
{
    char line[128]={0};           /*content of one line of config file*/
    char type[64]={0};            /*parameter type*/
    char value[64]={0};           /*parameter value*/
    char shell_command[64] = {0}; /*shell command*/
    char shell_result[64] = {0};  /*return result of shell command*/
    FILE *config_file;            /*config file pointer*/
       
    /*open config file*/
    config_file = fopen(CONFIG_FILEPATH, "r");
    if(!config_file){
      fprintf(stderr, "Config error: can not open config file!");
      return 0;
    }
        
    while(fgets(line, 128, config_file)){
      /*read parameters (skip comment line)*/
      if(line[0] != '#' && line[0] != ' ' && line[0] != '\n'){
        sscanf(line, "%s %s", type, value);
      }
      else{
        continue;
      }
      
      /*config parameters*/
      if(!strcmp(type, "CLOUD_IP")){
        /*config IP address of cloud platform*/
        strcpy(CLOUD_IP, value);
      }
      else if(!strcmp(type, "SINK_STA_TCP_PORT")){
        /*config TCP port of sink <-> STA communication*/
        SINK_STA_TCP_PORT = atoi(value);
      }
      else if(!strcmp(type, "ETH_IF")){
        /*config the name of the Ethernet Port (to internet)*/
        strcpy(ETH_IF, value);
      }
      else if(!strcmp(type, "MAX_STA_NUM")){
        /*config max number of STAs*/
        MAX_STA_NUM = atoi(value);
      }
      else{
        /*ignore other unknown parameters*/
        fprintf(stderr, "Config warning: no parameter named %s\n.", type);
      }
      
      /*clear all buffer*/
      memset(line, 0, sizeof(line));
      memset(type, 0, sizeof(type));
      memset(value, 0, sizeof(value));
    }
    
    /*Check Ethernet connection*/
    if(strlen(ETH_IF) == 0){
      fprintf(stderr, "Config error: ethernet port doesn't be assigned, check \'parameter.conf\'.\n");
      return 0;
    }
    memset(shell_command, 0, sizeof(shell_command));
    memset(shell_result, 0, sizeof(shell_result));
    sprintf(shell_command, "ifconfig %s|grep inet|grep -v 127.0.0.1|grep -v inet6|awk '{print $2}'|tr -d \"addr:\"", ETH_IF);
    get_system_output(shell_command, shell_result, 64);
    if(strlen(shell_result) == 0){
      fprintf(stderr, "Config error: %s has no ip address, check Ethernet connection!\n", ETH_IF);
      return 0;
    }else
      strcpy(ETH_IP, shell_result);

    /*Config IP address of WiFi AP*/
    memset(shell_result, 0, sizeof(shell_result));
    get_system_output("ifconfig wlan0|grep inet|grep -v 127.0.0.1|grep -v inet6|awk '{print $2}'|tr -d \"addr:\"", shell_result, 64);
    if(strlen(shell_result) == 0){
      fprintf(stdout, "Config info: wlan0 has no ip address, try to config.\n");
      return 0;
    }else
      strcpy(AP_IP, shell_result);
    
    fclose(config_file);
    return 1;
}

/*
   Function: print configure parameter
   Parameter: none
   Return: none
*/
void printf_config()
{
  fprintf(stdout, "Config info: CLOUD_IP is %s\n", CLOUD_IP);
  fprintf(stdout, "Config info: ETH_IP of %s is %s\n", ETH_IF, ETH_IP);
  fprintf(stdout, "Config info: AP_IP is %s\n", AP_IP);
  fprintf(stdout, "Config info: SINK_STA_TCP_PORT is %u\n", SINK_STA_TCP_PORT);
  fprintf(stdout, "Config info: MAX_STA_NUM is %d\n", MAX_STA_NUM);
  fprintf(stdout, "\n");
}

/*
   Function: get shell command's output
   Parameter: 
              cmd: input shell string
              output: output string
              size: don't set too large, basically 64 is enough
   Return: 0
*/
int get_system_output(char *cmd, char *output, int size)
{
    FILE *fp=NULL;  
    fp = popen(cmd, "r");   
    if(fp){   
      if(fgets(output, size, fp) != NULL){       
        if(output[strlen(output)-1] == '\n')            
            output[strlen(output)-1] = '\0'; 
      }   
      pclose(fp);
    }
    return 0;
}