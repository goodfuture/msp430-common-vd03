

#ifndef _AD_H
#define _AD_H

#include "system.h"
#include "prodata.h"

typedef struct _AD_PARAM
{
    INT8U id[4];    //��Ⱦ����
	INT8U type;     //AIת������4��20mA,0��5V,0��10V��
    float highval;  //��λֵ
    float lowval;   //��λֵ
    float rate;     //б��
    float offset;   //ƫ��
}AD_PARAM;

typedef struct _AD_CALIBPARAM{
    INT16U k20[8];  //20ma��У׼ֵ
    INT16U k4[8];   //4ma��У׼ֵ
}AD_CALIBPARAM;


extern AD_CALIBPARAM Ad_CalibParam;
extern AD_PARAM Ad_Param[8];
extern float Ad_RealData[8];
extern MAX_MIN_DATA Ad_MaxMin[8];

void ADParam_Calculate(void);
void ProcADC(void);

////////////////////////////////
float GetO2Val(char Index);

#endif
