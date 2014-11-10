

#include "system.h"
#include "dio.h"


#define DI1_ENABLE    P2OUT&=~BIT3
#define DI1_DISABLE   P2OUT|=BIT3
#define DI2_ENABLE    P2OUT&=~BIT4
#define DI2_DISABLE   P2OUT|=BIT4

#define CS_4245_EN    P2OUT&=~BIT5
#define CS_4245_DIS   P2OUT|=BIT5
#define DO_ENABLE     P2OUT&=~BIT0
#define DO_DISABLE    P2OUT|=BIT0

#define DOOUT         P1OUT
#define DIN           P1IN


INT8U PWR_Stat; 	//�е繩��״̬

INT32U DI_Val;		//���������Դ���16·DI��������16·
INT16U DO_Val;		//���������Դ���8·DO��������8·


INT16U Get_DIVal(void)
{
    INT16U value;
    
    DI1_ENABLE;
    value = DIN;
    DI1_DISABLE;
    Delay_N_mS(2);
    DI2_ENABLE;
    value |= (((INT16U)DIN)<<8);
    DI2_DISABLE;
    value =~ value;
    
    return value;
}

/****************************************
                DO OUTPUT
*****************************************/
void Set_DOVal(INT8U Val)
{
    CS_4245_EN;
    P1DIR=0XFF;   //direction output
    DO_ENABLE;
    Delay_N_mS(2);
    DOOUT = Val;
    Delay_N_mS(2);
    DO_DISABLE;
    CS_4245_DIS;
    P1DIR = 0X00;//direction input
}

void ProcDIO(void)
{
    INT16U dival;
	INT8U doval;
	
	dival = Get_DIVal();
	DI_Val = (DI_Val&0xffff0000) + dival;
	
	doval = (INT8U)DO_Val;
    Set_DOVal(doval);
}



