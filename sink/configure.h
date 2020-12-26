#ifndef _CONFIGURE_H_
#define _CONFIGURE_H_
#include <stdio.h>

/*--------------------Control Parameters--------------------*/
#define CONFIG_FILEPATH "./parameter.conf"
#define SET_STATIC_SINK_IP 1

/*--------------------Operation Functions--------------------*/
int config_parameter();
void printf_config();

/*--------------------Other Functions--------------------*/
int get_system_output(char *cmd, char *output, int size);

#endif
