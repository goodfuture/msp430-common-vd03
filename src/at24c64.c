


#include <string.h>
#include "system.h"
#include "at24c64.h"


#define AT24WR_PROTECT    P3OUT|=BIT0
#define AT24WR_UNPROTECT  P3OUT&=~BIT0

#define I2C_START     UCB0CTL1|=UCTXSTT
#define I2C_TRANSMIT  UCB0CTL1|=UCTR
#define I2C_RECEIVE   UCB0CTL1&=~UCTR
#define I2C_STOP      UCB0CTL1|=UCTXSTP


void AT24C64_W(void *src,INT16U des,INT16U len)
{
  volatile INT16U i;
  INT8U *Source;

  Source  =(INT8U *)src;
  I2C_TRANSMIT;   //transmit mode
  UCB0I2CSA =0x50;
  AT24WR_UNPROTECT;
  for(i=0;i<len;i++)
  {
    I2C_TRANSMIT;
    I2C_START;
    while((IFG2&UCB0TXIFG)==0);
    UCB0TXBUF =(INT8U)(des>>8);
    while((IFG2&UCB0TXIFG)==0);
    UCB0TXBUF =(INT8U)des;
    while((IFG2&UCB0TXIFG)==0);
    UCB0TXBUF =*Source++;
    while((IFG2&UCB0TXIFG)==0);
    I2C_STOP;
    while(UCB0CTL1&UCTXSTP);
    des++;
    Delay_N_mS(350);
  }
  AT24WR_PROTECT;
}

void AT24C64_RS(void *des,INT16U src,INT16U len)
{
  INT16U i;
  INT8U *Dest;
  Dest  =(INT8U *)des;

  I2C_TRANSMIT;   //transmit mode
  I2C_START;
  while((IFG2&UCB0TXIFG)==0);
  UCB0TXBUF =(INT8U)(src>>8);
  while((IFG2&UCB0TXIFG)==0);
  UCB0TXBUF =(INT8U)src;
  while((IFG2&UCB0TXIFG)==0);

  I2C_RECEIVE;
  I2C_START;

  while((IFG2&UCB0RXIFG)==0);
  *Dest =UCB0RXBUF;

  for(i=0;i<len;i++)
  {
    while((IFG2&UCB0RXIFG)==0);
    *Dest++ =UCB0RXBUF;
  }
  I2C_STOP;
}

////////////////////////////////

//程序版本号
void Read_VersionCode(char *Code)
{
    AT24C64_RS(Code,VERSIONCODE_ADDR,6);
    Delay_N_mS(500);
    AT24C64_RS(Code,VERSIONCODE_ADDR,6);
    Delay_N_mS(500);   
}

void Save_VersionCode(char *Code)
{
    char temp[6];
    while(1)    //写了再读，保证数据正确写入
    {
        AT24C64_W(Code,VERSIONCODE_ADDR,6);
              
        Read_VersionCode(temp);
        if(memcmp(temp,Code,6) == 0)
            break;
    }
}

//系统参数
void Default_SysParam(SYS_PARAM *Param)
{
    strcpy(Param->pw,"123456");   
    strcpy(Param->st,"32");
    strcpy(Param->mn,"12345678901234");
    strcpy(Param->sim,"00000000000");
    strcpy(Param->rtd,"030");
    strcpy(Param->pw,"123456");
	
	//需要修改
	Param->worktype = WORKTYPE_AUTO;
    Param->pumpen = 0x03;
    Param->engineen = 0x03;
    Param->valueen = 0x03;
	Param->backflowen = 0x03;
    Param->reuseen = 0x03;
	Param->levelen = 0x0f;
	
	Param->dohigh = 3.2;
	Param->dolow = 1.9;
    Param->engine_switchlong = 360;
    Param->value_switchlong = 360;
    Param->value_switchlast = 15;
}


void Read_SysParam(SYS_PARAM *Param)
{
    AT24C64_RS(Param,SYSPARAM_ADDR,sizeof(SYS_PARAM));
    Delay_N_mS(500);
    AT24C64_RS(Param,SYSPARAM_ADDR,sizeof(SYS_PARAM));
    Delay_N_mS(500);   
}

void Save_SysParam(SYS_PARAM *Param)
{
    SYS_PARAM temp;
    while(1)
    {
        AT24C64_W(Param,SYSPARAM_ADDR,sizeof(SYS_PARAM));
              
        Read_SysParam(&temp);
        if(memcmp(&temp,Param,sizeof(SYS_PARAM)) == 0)
            break;
    } 
}

//AD参数
void Default_ADParam(AD_PARAM *Param)
{
    char i;
    //需要修改
    for(i=0;i<8;++i)
    {
        if(i == 0)
            strcpy((char *)Param[i].id,"O21");
		else if(i == 1)
            strcpy((char *)Param[i].id,"FU1");
		else if(i == 2)
            strcpy((char *)Param[i].id,"FU2");
		else if(i == 3)
            strcpy((char *)Param[i].id,"FU3");
        else
            strcpy((char *)Param[i].id,"000");
        Param[i].type = 0;   
        Param[i].highval = 60;
        Param[i].lowval = 0;
        Param[i].rate = 0;
        Param[i].offset = 0;
    }
}

void Read_ADParam(AD_PARAM *Param)
{
    AT24C64_RS(Param,ADPARAM_ADDR,sizeof(AD_PARAM)*8);
    Delay_N_mS(500);
    AT24C64_RS(Param,ADPARAM_ADDR,sizeof(AD_PARAM)*8);
    Delay_N_mS(500);   
}

void Save_ADParam(AD_PARAM *Param)
{
    AD_PARAM temp[8];
    while(1)
    {
        AT24C64_W(Param,ADPARAM_ADDR,sizeof(AD_PARAM)*8);
              
        Read_ADParam(temp);
        if(memcmp(temp,Param,sizeof(AD_PARAM)*8) == 0)
            break;
    } 
}

//通信参数
void Default_ComParam(COM_PARAM *Param)
{
    char i;
    //需要修改
    Param->baud_index = 0;
    for(i=0;i<MODBUS_VARNUM;++i)
    {    
        if(i == 0)
        {
            strcpy((char *)Param->regid[i],"ECS");
            Param->regaddr[i] = 1+0x14;
			Param->devid[i] = 1;
        }
		else if(i == 1)
        {
            strcpy((char *)Param->regid[i],"UAP");
            Param->regaddr[i] = 1+0x40;
            Param->devid[i] = 1;
        }
		/*else if(i == 2)
        {
            strcpy((char *)Param->regid[i],"IAP");
            Param->regaddr[i] = 1+0x43;
			Param->devid[i] = 1;
        }
        else if(i == 3)
        {
            strcpy((char *)Param->regid[i],"UBP");
            Param->regaddr[i] = 1+0x41;
            Param->devid[i] = 1;
        }
      	else if(i == 4)
        {
            strcpy((char *)Param->regid[i],"IBP");
            Param->regaddr[i] = 1+0x44;
            Param->devid[i] = 1;
        }
        else if(i == 5)
        {
            strcpy((char *)Param->regid[i],"UCP");
            Param->regaddr[i] = 1+0x42;
            Param->devid[i] = 1;
        }
      	else if(i == 6)
        {
            strcpy((char *)Param->regid[i],"ICP");
            Param->regaddr[i] = 1+0x45;
            Param->devid[i] = 1;
        }*/
        else
        {
            strcpy((char *)Param->regid[i],"000");
            Param->regaddr[i] = 1;
			Param->devid[i] = 1;
        }
    }
}

void Read_ComParam(COM_PARAM *Param)
{
    AT24C64_RS(Param,COMPARAM_ADDR,sizeof(COM_PARAM));
    Delay_N_mS(500);
    AT24C64_RS(Param,COMPARAM_ADDR,sizeof(COM_PARAM));
    Delay_N_mS(500);   
}

void Save_ComParam(COM_PARAM *Param)
{
    COM_PARAM temp;
    while(1)
    {
        AT24C64_W(Param,COMPARAM_ADDR,sizeof(COM_PARAM));
              
        Read_ComParam(&temp);
        if(memcmp(&temp,Param,sizeof(COM_PARAM)) == 0)
            break;
    } 
}

//对AD校准参数进行修正
void Fix_ADCalibParam(AD_CALIBPARAM *Param)
{
    char i;
    
    for(i=0;i<8;++i)
    {
        if((Param->k4[i]<0x100) || (Param->k20[i]>0x850))
        {
            Param->k4[i] =0x19b;
            Param->k20[i] =0x7f9;
        }
    }
}

void Read_ADCalibParam(AD_CALIBPARAM *Param)
{
    AT24C64_RS(Param,ADCALIBPARAM_ADDR,sizeof(AD_CALIBPARAM));
    Delay_N_mS(500);
    AT24C64_RS(Param,ADCALIBPARAM_ADDR,sizeof(AD_CALIBPARAM));
    Delay_N_mS(500);   
}

void Save_ADCalibParam(AD_CALIBPARAM *Param)
{
    AD_CALIBPARAM temp;
    while(1)
    {
        AT24C64_W(Param,ADCALIBPARAM_ADDR,sizeof(AD_CALIBPARAM));
              
        Read_ADCalibParam(&temp);
        if(memcmp(&temp,Param,sizeof(AD_CALIBPARAM)) == 0)
            break;
    }    
} 

