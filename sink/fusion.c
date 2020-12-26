#include "fusion.h"

char ret_val_to_term[10];            /*return value to terminal*/

/*-------------------------Fusion related-------------------------*/
/*
 * 	Function: form membership (构造隶属度函数)
 * 	Parameter:
                data:   ...
                x1:     ...
                x2:     ...
                x3:     ...
                y1:     ...
                y2:     ...
 * 	Return: 
                membership
 */
float Membership(float data, float x1, float x2, float x3, float y1, float y2)
{
    float pro = 0.0;
    if (data < x1){
        pro = y1;
    }
    else if (x1 <= data < x2){
        pro = ((data - x1) * (y2 - y1) / (x2 - x1)) + y1;
    }
    else if (x2 <= data < x3){
        pro = ((data - x2) * (1 - y2) / (x3 - x2)) + y2;
    }
    else{
        pro = 1;
    }
    return pro;
}

/*
 * 	Function: data_fusion
 * 	Parameter:      
                Fire_data: buffer that stores fire_data
 * 	Return: 
                fusion result
 */
float data_fusion(float Fire_data[])
{
    float Tem_data_membership, Smog_data_membership, CO_data_membership;
    float data_fusion = 0.0;

    Tem_data_membership = Membership(Fire_data[0], 30, 55, 80, 0.1, 0.5);       //温度的隶属度
    Smog_data_membership = Membership(Fire_data[1], 300, 1000, 4000, 0.1, 0.5); //烟雾的隶属度
    CO_data_membership = Membership(Fire_data[2], 30, 100, 200, 0.1, 0.5);      //CO的隶属度

    // printf("tem:%.2f %.2f\n", Tem_data_membership, 1 - Tem_data_membership);
    // printf("smog:%.2f %.2f\n", Smog_data_membership, 1 - Smog_data_membership);
    // printf("CO:%.2f %.2f\n", CO_data_membership, 1 - CO_data_membership);

    data_fusion = (Tem_data_membership + Smog_data_membership + CO_data_membership) / 3; //有火灾发生的概率

    return data_fusion;
}

/*
 * 	Function: calculate the mean of float data
 * 	Parameter:
                data: float data
                length: the length of float data
 * 	Return:     
                the mean of data
 */
double mean(double data[], int length)
{
    int i;
    double sum = 0, aver;
    for (i = 0; i < length; i++)
        sum += data[i];
    aver = sum / length;
    return aver;
}

/*-------------------------Control related-------------------------*/
/*
 * 	Function: cooperative_control
 * 	Parameter:
                fusion_result: fusion data
 * 	Return: 
                smaple frequency of sensor
 */
int cooperative_control(float fusion_result)
{
    int fre;
    fre=fusion_result*1000;
    return fre;
}

