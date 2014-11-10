


#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <msp430xG46x.h>


//�������Ͷ���
typedef unsigned char INT8U;
typedef unsigned int  INT16U;
typedef unsigned long INT32U;

typedef struct _tm{
    int year;
    int month;
    int date;
    int hour;
    int minute;
    int second;
    int weekday;
}T_Times,*T_pTimes;

#define ENABLEINT()   _BIS_SR(GIE)
#define DISABLEINT()  _BIC_SR(GIE)


#define MS2TENMS(x) ((x)/10)      //������ֵת����10msֵ
#define S2TENMS(x)  ((x)*100)     //����ֵת����10msֵ
#define MIN2S(x)    ((x)*60)      //������ֵת������ֵ


extern const char VersionCode[6];
extern INT8U SoftWDT_Flag;
extern INT8U LEDTimer;
extern INT8U RTCTimer;
extern INT16U RTCErrTimer;
extern INT8U System_TimeStr[16];
extern INT8U System_StartTimeStr[16];
extern T_Times Current_Tm;


//��������
void CPU_Init(void);
void Delay_N_mS(unsigned int n_milisecond);
void Delay_10_uS(void);

void Start_WDT(void);
void Stop_WDT(void);
void Feed_WDT(void);

#endif
