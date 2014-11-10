

#ifndef _MODBUS_H
#define _MODBUS_H

#include "uart.h"
#include "prodata.h"

#define MODBUS_COM  COM5
//#define MODBUS_COM  COM4
#define MODBUS_VARNUM 7


typedef struct _COM_PARAM
{
    INT8U baud_index;
    INT8U devid[MODBUS_VARNUM];
    INT16U regaddr[MODBUS_VARNUM];
    char regid[MODBUS_VARNUM][4];
} COM_PARAM;


extern COM_PARAM Com_Param;
//const char ComBaud_Str[4][6] = {"9600","19200","2400","4800"};
extern const char ComBaud_Str[4][6];
extern float Com_Val[MODBUS_VARNUM];
extern MAX_MIN_DATA Com_MaxMin[MODBUS_VARNUM];

extern INT8U MbOtCnt;
extern INT16U MbScanCnt;

void Modbus_Init(void);
void ProcessModbus(void);

#endif //end of _MODBUS_H

