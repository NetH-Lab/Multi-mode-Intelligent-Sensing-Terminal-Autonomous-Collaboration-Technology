#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define kprintf printf
#define int32_t int
#define uint32_t int
#define MAX_AD 16384
#define MAX_2AD 32768
#define RUN_COUNT 10000000
#define PCRAM_DEFAULT_VALUE 7877

#define SIN1U 	0.2588190451		//sin(15)
#define SIN2U 	0.5					//sin(30)
#define SIN3U 	0.7071067812		//sin(45)
#define SIN4U 	0.8660254038		//sin(60)
#define SIN5U 	0.9659258263		//sin(75)


int32_t on_run_TUNIT_Forier_Full_Base(int i, int kind);
int32_t on_run_TUNIT_Forier_Full_Pcram(int i, int kind);
// 从pcram中取值
double res(int i, int j);

double pcram[MAX_AD*2-1][5];//32767
double y_val[5] = {SIN1U, SIN2U, SIN3U, SIN4U, SIN5U};
//元件执行接口
int32_t on_run_TUNIT_Forier_Full_Base(int i, int kind)
{
    uint32_t i_real = 0;
    uint32_t i_image = 0;
	if(kind==1) {

    double real = 0;
    double image = 0;
    int32_t ac[24] = {0,26,50,71,87,96,100,96,87,71,50,26,0,-26,-50,-71,-87,-96,-100,-96,-87,-71,-50,-26};


    /************************************************************************/
    //		24点采样付氏计算
    /************************************************************************/
    int32_t temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7 , temp8, temp9, temp10, temp11;
    	temp0 = ac[0] - ac[12];
    	temp1 = ac[1] - ac[13];
    	temp2 = ac[2] - ac[14];
    	temp3 = ac[3] - ac[15];
    	temp4 = ac[4] - ac[16];
    	temp5 = ac[5] - ac[17];
    	temp6 = ac[6] - ac[18];
    	temp7 = ac[7] - ac[19];
    	temp8 = ac[8] - ac[20];
    	temp9 = ac[9] - ac[21];
    	temp10 = ac[10] - ac[22];
    	temp11 = ac[11] - ac[23];

    	real  = ((temp1+temp11) * SIN1U + (temp2+temp10) * SIN2U + (temp3+temp9) * SIN3U + (temp4+temp8) * SIN4U + (temp5+temp7) * SIN5U + temp6)/12.0;
    	image = ((temp1-temp11) * SIN5U + (temp2-temp10) * SIN4U + (temp3-temp9) * SIN3U + (temp4-temp8) * SIN2U + (temp5-temp7) * SIN1U + temp0)/12.0;


 //   	OUTPUT_cpx.Real = real * PRI_param;
 //   	OUTPUT_cpx.Image = image * PRI_param;

			i_real = (uint32_t)(10000.0 * real);
			i_image = (uint32_t)(10000.0 * image);
	}

	if(i==RUN_COUNT-1) {
			kprintf("method 1 Real = %d ! Image = %d\n\r", i_real, i_image);
	}
//	result[i%10000] = i_real;
    return 1;
}

//元件执行接口,打表法
int32_t on_run_TUNIT_Forier_Full_Pcram(int i, int kind)
{
    uint32_t i_real = 0;
    uint32_t i_image = 0;
	if(kind==1) {

    double real = 0;
    double image = 0;
    int32_t ac[24] = {0,26,50,71,87,96,100,96,87,71,50,26,0,-26,-50,-71,-87,-96,-100,-96,-87,-71,-50,-26};


    /************************************************************************/
    //		24点采样付氏计算
    /************************************************************************/
    int32_t temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7 , temp8, temp9, temp10, temp11;
    	temp0 = ac[0] - ac[12];
    	temp1 = ac[1] - ac[13];
    	temp2 = ac[2] - ac[14];
    	temp3 = ac[3] - ac[15];
    	temp4 = ac[4] - ac[16];
    	temp5 = ac[5] - ac[17];
    	temp6 = ac[6] - ac[18];
    	temp7 = ac[7] - ac[19];
    	temp8 = ac[8] - ac[20];
    	temp9 = ac[9] - ac[21];
    	temp10 = ac[10] - ac[22];
    	temp11 = ac[11] - ac[23];

    	real  = (   res((temp1+temp11)  , 0)
              +     res((temp2+temp10)  , 1)
              +     res((temp3+temp9)   , 2)
              +     res((temp4+temp8)   , 3)
              +     res((temp5+temp7)   , 4)
              +     temp6)/12.0;

    	image = (   res((temp1-temp11)  , 4)
              +     res((temp2-temp10)  , 3)
              +     res((temp3-temp9)   , 2)
              +     res((temp4-temp8)   , 1)
              +     res((temp5-temp7)   , 0)
              +     temp0)/12.0;


 //   	OUTPUT_cpx.Real = real * PRI_param;
 //   	OUTPUT_cpx.Image = image * PRI_param;

			i_real = (uint32_t)(10000.0 * real);
			i_image = (uint32_t)(10000.0 * image);
	}

	if(i==RUN_COUNT-1) {
			kprintf("method 2 Real = %d ! Image = %d\n\r", i_real, i_image);
	}
//	result[i%10000] = i_real;
    return 1;
}

int times = 0;

// 从pcram中取值
double res(int i, int j)
{
    double dv = 0.0;
    int x, y;
    if(i>=0 && i<MAX_AD) {
        x = i;
    } else if(i<=-1 && i>=-MAX_AD) {//-1==>32767;-16384==>16384
        x = MAX_2AD + i;
    } else {
        kprintf("index x over:%d", i);
        return 0;
    }
    if(j>=0 && j<=4) {
        y = j;
    } else {
        kprintf("index y over:%d", j);
        return 0;
    }

    dv = pcram[x][y];
    if(PCRAM_DEFAULT_VALUE==(int)dv) {
        kprintf("load[%d,%d]=%f\t",i,j,dv);
        times++;
        if(times>=100)getchar();
        dv = i * y_val[y];
        pcram[x][y] = dv;
        kprintf("save[%d,%d]=%f\n",i,j,dv);
    }
    return dv;
}

void clean_pcram_array()
{
    int i,j;
    for(i=0;i<MAX_2AD;i++)
    {
        for(j=0;j<5;j++)
        {
            pcram[i][j] = PCRAM_DEFAULT_VALUE;
        }
    }
}

#define clock_res (int)(t_2-t_1)
int main()
{
    printf("Hello Rudy FFT!\n");
    //二维数组清零
    clean_pcram_array();

    int ii;
    time_t t_1,t_2;

    t_1 = clock();
    for(ii=0;ii<RUN_COUNT;ii++) {
        on_run_TUNIT_Forier_Full_Base(ii, 1);
	}
    t_2 = clock();
    kprintf("method 1 cost ms : %d\n", clock_res);

    t_1 = clock();
    for(ii=0;ii<RUN_COUNT;ii++) {
        on_run_TUNIT_Forier_Full_Pcram(ii, 1);
	}
    t_2 = clock();
    kprintf("method 2 cost ms : %d\n", clock_res);

    int ctime;
    t_1 = clock();
    for(ii=0;ii<RUN_COUNT*100;ii++) {
	}
    t_2 = clock();
    ctime = clock_res;
    kprintf("only circle cost ms : %d\n", ctime);

    t_1 = clock();
    for(ii=0;ii<RUN_COUNT*100;ii++) {
        3.1415926 * 9.87654;
	}
    t_2 = clock();
    kprintf("only multi - circle cost ms : %d\n", clock_res - ctime);

    t_1 = clock();
    for(ii=0;ii<RUN_COUNT*100;ii++) {
        pcram[MAX_AD][4];
	}
    t_2 = clock();
    kprintf("only load - circle cost ms : %d\n", clock_res - ctime);

    t_1 = clock();
    for(ii=0;ii<RUN_COUNT*100;ii++) {
        123 + 654;
	}
    t_2 = clock();
    kprintf("only plus - circle cost ms : %d\n", clock_res - ctime);


    return 0;
}
