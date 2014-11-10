

#include "system.h"
#include "ad.h"
#include "string.h"
#include "stdio.h"

AD_CALIBPARAM Ad_CalibParam;
AD_PARAM Ad_Param[8];
float Ad_RealData[8];
MAX_MIN_DATA Ad_MaxMin[8];

static INT16U Ad_Value[8];
static INT16U Ad_Result[8][10];
static INT8U Ad_ConvertEnd=0;
static INT8U Ad_Times=0;


//����У׼���ݼ�����ص�У׼��
void ADParam_Calculate(void)
{
    INT16U i;
    float k4tmp;
    float k20_k4,su_sd;
    
    for(i=0;i<8;i++)
    {
        if(Ad_Param[i].type == 0)
        {
            //�����������4mA�����ֵ��У׼��ֵ��2����
            //Ad_Calib_Param.k4[i] += 2;
            k4tmp = (float)Ad_CalibParam.k4[i];
            k20_k4 =(float)(Ad_CalibParam.k20[i]-k4tmp);
            su_sd =Ad_Param[i].highval-Ad_Param[i].lowval;
            Ad_Param[i].rate =su_sd/k20_k4;
            Ad_Param[i].offset =((5*k4tmp-Ad_CalibParam.k20[i])*su_sd)/
                (4*k20_k4)-(5*Ad_Param[i].lowval-Ad_Param[i].highval)/4;    
        }
        else
        {
            Ad_Param[i].rate=(Ad_Param[i].highval-Ad_Param[i].lowval)/0x1000;
        }
    } 
}

/*******************************************
          AD interrupt function
*******************************************/
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    Ad_Result[0][2*Ad_Times]=ADC12MEM0;
    Ad_Result[0][2*Ad_Times+1]=ADC12MEM1;
    Ad_Result[1][2*Ad_Times]=ADC12MEM2;
    Ad_Result[1][2*Ad_Times+1]=ADC12MEM3;
    Ad_Result[2][2*Ad_Times]=ADC12MEM4;
    Ad_Result[2][2*Ad_Times+1]=ADC12MEM5;
    Ad_Result[3][2*Ad_Times]=ADC12MEM6;
    Ad_Result[3][2*Ad_Times+1]=ADC12MEM7;
    Ad_Result[4][2*Ad_Times]=ADC12MEM8;
    Ad_Result[4][2*Ad_Times+1]=ADC12MEM9;
    Ad_Result[5][2*Ad_Times]=ADC12MEM10;
    Ad_Result[5][2*Ad_Times+1]=ADC12MEM11;
    Ad_Result[6][2*Ad_Times]=ADC12MEM12;
    Ad_Result[6][2*Ad_Times+1]=ADC12MEM13;
    Ad_Result[7][2*Ad_Times]=ADC12MEM14;
    Ad_Result[7][2*Ad_Times+1]=ADC12MEM15;
    
    //ÿ�����ݲ���ʮ��
    Ad_Times++;
    if(Ad_Times>=5)
    {
        ADC12CTL0&=~(ADC12SC+ENC);  //�ر�ADת��
        Ad_Times=0;
        Ad_ConvertEnd=1;
    }
}
/*******************************************
              AD Converter
*******************************************/
void AD_ReadResult(void)
{
    INT8U i,j;
    INT16U *max,*min,*mid;
    
    for(i=0;i<8;i++)
    {
        Ad_Value[i]=0;
        max=&Ad_Result[i][0];
        min=max;
        mid=&Ad_Result[i][1];
        for(j=0;j<9;j++)           //����������ֵ
        {
            if(*max<*mid)
                max=mid;
            if(*min>*mid)
                min=mid;
            mid=&Ad_Result[i][j+2];
        }
        if(max==min)
            min+=1;
        *max=0;      //ȥ�����ֵ
        *min=0;      //ȥ�����ֵ
        
        for(j=0;j<10;j++)
            Ad_Value[i]+=Ad_Result[i][j];
        Ad_Value[i]=(Ad_Value[i]>>3);   //ȡƽ��ֵ
    }
}

//����У׼���������ʵ�ʵĲ���ֵ
void AD_Calculation(void)
{
    INT8U i;
    INT16U AD_f;
    float AD_k;
    
    for(i=0;i<8;i++)
    {
        AD_f=Ad_Value[i];
        switch(Ad_Param[i].type)
        {
        case 0:
            if(AD_f<Ad_CalibParam.k4[i])
              AD_f =Ad_CalibParam.k4[i];
            AD_k =Ad_Param[i].rate*AD_f-Ad_Param[i].offset;
            break;
        case 1:
            AD_k=Ad_Param[i].rate*AD_f+Ad_Param[i].lowval;	
            break;
        case 2:
            AD_k=Ad_Param[i].rate*AD_f*2+Ad_Param[i].lowval;
            break;
        default:
            break;
        }
        
        if(AD_k>Ad_Param[i].highval)
          Ad_RealData[i]=Ad_Param[i].highval;
        else
          Ad_RealData[i]=AD_k;
    }
}

//�����Դ���ADC��������
void ProcADC(void)
{
    if(Ad_ConvertEnd)
    {	
        Ad_ConvertEnd=0;
        AD_ReadResult();		    //��ADת������
        AD_Calculation();           //����ADת��ֵ
        ADC12CTL0|=ADC12SC+ENC;     //���¿���ADת��
    }  
}

///////////////////////////////////////////
float GetO2Val(char Index)
{
	char i;
	char str[8];
	
	sprintf(str, "O2%01d", Index+1);
	for(i=0;i<8;++i)
	{
		if(memcmp(Ad_Param[i].id,str,3) == 0)
		{
			return Ad_RealData[i]; 
		}
	}
	
	return 0;
}
