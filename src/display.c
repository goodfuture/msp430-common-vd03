

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "lcd.h"
#include "dio.h"
#include "ad.h"
#include "at24c64.h"
#include "control.h"
#include "ds1302.h"
#include "modbus.h"


//各个显示画面的索引编号
#define WELCOME_INDEX       0
#define MAINSEL_INDEX       1
#define PWINPUT_INDEX       2
#define STATWATCH_INDEX     3
#define DATAWATCH_INDEX     4
#define LOGWATCH_INDEX      5
#define CONTROL_INDEX       6
#define SETUP_INDEX         7
#define WORKSETUP_INDEX     8
#define DEVSETUP_INDEX      9
#define SYSSETUP_INDEX      10
#define AISETUP_INDEX       11
#define COMSETUP_INDEX      12


INT8U DisplayTimer;         //定时刷新计数器
INT16U BackLightTimer;      //背光时间计数器


static char Dis_AllRefresh = 0;     //整屏刷新标志
static char Dis_PartRefresh = 0;    //部分屏幕刷新标志
static char MainDis_Index = 0;      //画面显示索引
static char SndDis_Index = 0;       //子画面显示索引
static char PWLast_Index = 0;       //密码输入之前的窗口索引
static char ThdDis_Index = 0;       //子子画面索引

static char Input_Temp[24];         //临时输入数据
static char Input_Pos;              //光标位置
static char Input_Type;             //显示状态    

//显示部分
//显示欢迎画面
void Dis_Welcome(INT8U AllRefresh)  
{
	INT8U indexp[12];
	INT8U k;
    
    static INT8U index_wel[] = {
        1,2,3,4,5,6,7,8,9,10,0   
    };
    static INT8U index_date[] = {
        11,12,0   
    };
    static INT8U index_time[] = {
        13,14,0   
    };
	
    if(AllRefresh)
    {
        //欢迎字符串
        Clr_LCDRam();
        Disp_Chn1616_Str(4,35,index_wel,1);
        Disp_Xline(52);
        Disp_Xline(54);
            
        //版本号
        if((VersionCode[0] == 'V') && (VersionCode[1] == 'A'))
        {
            indexp[0] = 16;   
            indexp[1] = 22;
            indexp[2] = (VersionCode[2]&0x0f)+1;
            indexp[3] = (VersionCode[3]&0x0f)+1;
            indexp[4] = 0;
        }
		else if((VersionCode[0] == 'V') && (VersionCode[1] == 'D'))
        {
            indexp[0] = 16;   
            indexp[1] = 21;
            indexp[2] = (VersionCode[2]&0x0f)+1;
            indexp[3] = (VersionCode[3]&0x0f)+1;
            indexp[4] = 0;
        }
        else
        {
            indexp[0] = 26;   
            indexp[1] = 26;
            indexp[2] = 26;   
            indexp[3] = 26;
            indexp[4] = 0;
        }
        Disp_Chn816_Str(26,55,indexp,1);
            
        Disp_Chn1616_Str(8,80,index_date,1);
        Disp_Chn1616_Str(8,98,index_time,1);
        
        //：号和-号
        Disp_Chn816_Char(18,80,13,1);
        Disp_Chn816_Char(21,80,13,1);
        Disp_Chn816_Char(16,98,14,1);
        Disp_Chn816_Char(19,98,14,1);
    }
    
    if(!AllRefresh)
    {
        Clr_RamBlock(14,80,4);
        Clr_RamBlock(19,80,2);
        Clr_RamBlock(22,80,2);
        Clr_RamBlock(14,98,2);
        Clr_RamBlock(17,98,2);
        Clr_RamBlock(20,98,2);
    }
    
    //日期和时间的数值    
    for(k=0;k<4;k++)
		indexp[k]=(System_TimeStr[k]&0x0f)+1;
    indexp[4]= 0;
    Disp_Chn816_Str(14,80,indexp,1);
    for(k=0;k<2;k++)
        indexp[k]=(System_TimeStr[k+4]&0x0f)+1;
    indexp[2]= 0;
    Disp_Chn816_Str(19,80,indexp,1);
    for(k=0;k<2;k++)
        indexp[k]=(System_TimeStr[k+6]&0x0f)+1;
    indexp[2]= 0;
    Disp_Chn816_Str(22,80,indexp,1);
    
    for(k=0;k<2;k++)
        indexp[k]=(System_TimeStr[k+8]&0x0f)+1;
    indexp[2]= 0;
    Disp_Chn816_Str(14,98,indexp,1);
    for(k=0;k<2;k++)
        indexp[k]=(System_TimeStr[k+10]&0x0f)+1;
    indexp[2]= 0;
    Disp_Chn816_Str(17,98,indexp,1);
    for(k=0;k<2;k++)
        indexp[k]=(System_TimeStr[k+12]&0x0f)+1;
    indexp[2]= 0;
    Disp_Chn816_Str(20,98,indexp,1);
}

//显示主菜单
void Dis_MainSel(INT8U AllRefresh)  
{
    static INT8U index_menu[] = {
        15,16,17,18,0  
    };
    static INT8U index_watch[] = {
        19,20,21,22,0  
    };
    static INT8U index_setup[] = {
        27,28,29,30,0  
    };
	static INT8U index_dotest[] = {
        21,19,32,29,17,32,0  
    };
        
    if(AllRefresh)
    {
        SndDis_Index = 0;
        Clr_LCDRam();
		Disp_Chn1616_Str(11,20,index_menu,1);
        Disp_Chn1616_Str(11,50,index_watch,0);
		Disp_Chn1616_Str(11,70,index_setup,1); 
		Disp_Chn816_Str(12,90,index_dotest,1);
    }
    else
    {
        if(SndDis_Index == 0)
        {
            Disp_Chn1616_Str(11,50,index_watch,0);
		    Disp_Chn1616_Str(11,70,index_setup,1); 
			Disp_Chn816_Str(12,90,index_dotest,1);
        }
        else if(SndDis_Index == 1)
        {
            Disp_Chn1616_Str(11,50,index_watch,1);
		    Disp_Chn1616_Str(11,70,index_setup,0); 
			Disp_Chn816_Str(12,90,index_dotest,1);
        }
		else if(SndDis_Index == 2)
        {
            Disp_Chn1616_Str(11,50,index_watch,1);
		    Disp_Chn1616_Str(11,70,index_setup,1); 
			Disp_Chn816_Str(12,90,index_dotest,0);
        }
    }
}

//设备状态
void Dis_StatWatch(INT8U AllRefresh)  
{
    static INT8U index_stat[] = {
        19,20,0  
    };
    static INT8U index_data[] = {
        28,31,0  
    };
    static INT8U index_log[] = {
        34,19,35,0  
    };
    static INT8U index_worktype[] = {
        32,33,34,35,0  
    };
     static INT8U index_pump[] = {
        36,37,0  
    };
    static INT8U index_engine[] = {
        39,40,0  
    };
    static INT8U index_value[] = {
        103,104,0  
    };
	static INT8U index_backflow[] = {
        123,124,0  
    };
	static INT8U index_reuse[] = {
        88,80,0  
    };
	static INT8U index_pumplevel[] = {
        36,37,42,43,0  
    };
	static INT8U index_backflowlevel[] = {
        124,42,43,0  
    };
	static INT8U index_reuselevel[] = {
        88,80,42,43,0  
    };
    static INT8U index_disable[] = {
        106,4,0  
    };
    static INT8U index_manual[] = {
        23,24,0  
    };
    static INT8U index_auto[] = {
        44,24,0  
    };
	static INT8U index_remote[] = {
        100,72,0  
    };
    static INT8U index_stop[] = {
        45,46,0  
    };
    static INT8U index_run[] = {
        47,48,0  
    };
    static INT8U index_fault[] = {
        49,50,0  
    };
    static INT8U index_open[] = {
        119,62,0  
    };
    static INT8U index_close[] = {
        120,121,0  
    };
	    
    if(AllRefresh)
    {
        //显示固定不变的内容
        Clr_LCDRam(); 
        Disp_Yline(4,0);
        
        Disp_Chn1616_Str(0,15,index_stat,0);
        Disp_Chn1616_Str(0,35,index_data,1);
        Disp_Chn816_Str(0,55,index_log,1);
        
        Disp_Chn1616_Str(5,0,index_worktype,1);
        Disp_Chn816_Char(13,0,14,1);
        
        Disp_Chn816_Char(5,16,1,1);
        Disp_Chn816_Char(6,16,35,1);
        Disp_Chn1616_Str(7,16,index_pump,1);
        Disp_Chn816_Char(11,16,14,1);
        Disp_Chn816_Char(18,16,2,1);
        Disp_Chn816_Char(19,16,35,1);
        Disp_Chn1616_Str(20,16,index_pump,1);
        Disp_Chn816_Char(24,16,14,1);
        
        Disp_Chn816_Char(5,32,1,1);
        Disp_Chn816_Char(6,32,35,1);
        Disp_Chn1616_Str(7,32,index_engine,1);
        Disp_Chn816_Char(11,32,14,1);
        Disp_Chn816_Char(18,32,2,1);
        Disp_Chn816_Char(19,32,35,1);
        Disp_Chn1616_Str(20,32,index_engine,1);
        Disp_Chn816_Char(24,32,14,1);
        
        Disp_Chn816_Char(5,48,1,1);
        Disp_Chn816_Char(6,48,35,1);
        Disp_Chn1616_Str(7,48,index_value,1);
        Disp_Chn816_Char(11,48,14,1);
        Disp_Chn816_Char(18,48,2,1);
        Disp_Chn816_Char(19,48,35,1);
        Disp_Chn1616_Str(20,48,index_value,1);
        Disp_Chn816_Char(24,48,14,1);
		
		Disp_Chn816_Char(5,64,1,1);
        Disp_Chn816_Char(6,64,35,1);
        Disp_Chn1616_Str(7,64,index_backflow,1);
        Disp_Chn816_Char(11,64,14,1);
        Disp_Chn816_Char(18,64,2,1);
        Disp_Chn816_Char(19,64,35,1);
        Disp_Chn1616_Str(20,64,index_backflow,1);
        Disp_Chn816_Char(24,64,14,1);
        
        Disp_Chn816_Char(5,80,1,1);
        Disp_Chn816_Char(6,80,35,1);
        Disp_Chn1616_Str(7,80,index_reuse,1);
        Disp_Chn816_Char(11,80,14,1);
        Disp_Chn816_Char(18,80,2,1);
        Disp_Chn816_Char(19,80,35,1);
        Disp_Chn1616_Str(20,80,index_reuse,1);
        Disp_Chn816_Char(24,80,14,1);
		
		Disp_Chn1616_Str(5,96,index_pumplevel,1);
        Disp_Chn816_Char(13,96,14,1);
		Disp_Chn1616_Str(18,96,index_reuselevel,1);
        Disp_Chn816_Char(26,96,14,1);
		
		Disp_Chn816_Char(5,112,1,1);
        Disp_Chn816_Char(6,112,35,1);
        Disp_Chn1616_Str(7,112,index_backflowlevel,1);
        Disp_Chn816_Char(13,112,14,1);
        Disp_Chn816_Char(18,112,2,1);
        Disp_Chn816_Char(19,112,35,1);
        Disp_Chn1616_Str(20,112,index_backflowlevel,1);
        Disp_Chn816_Char(26,112,14,1);
	}
    
    if(!AllRefresh)
    {
        //Clr_RamBlock(14,20,6); 
        //Clr_RamBlock(22,20,8);
	}
    
    //获得各状态并显示
    if(Get_WorkType() == WORKTYPE_AUTO)
        Disp_Chn1616_Str(14,0,index_auto,1); 
    else if(Get_WorkType() == WORKTYPE_MANUAL)
        Disp_Chn1616_Str(14,0,index_manual,1);
	else 
        Disp_Chn1616_Str(14,0,index_remote,1); 
	
	//提升泵状态  
    if(Get_PumpStat(0) == DEVICE_DISABLE)
        Disp_Chn1616_Str(12,16,index_disable,1); 
    else if(Get_PumpStat(0) == DEVICE_STOP)
        Disp_Chn1616_Str(12,16,index_stop,1); 
    else if(Get_PumpStat(0) == DEVICE_RUN)
        Disp_Chn1616_Str(12,16,index_run,1);
    else
        Disp_Chn1616_Str(12,16,index_fault,1);
    
    if(Get_PumpStat(1) == DEVICE_DISABLE)
        Disp_Chn1616_Str(25,16,index_disable,1); 
    else if(Get_PumpStat(1) == DEVICE_STOP)
        Disp_Chn1616_Str(25,16,index_stop,1); 
    else if(Get_PumpStat(1) == DEVICE_RUN)
        Disp_Chn1616_Str(25,16,index_run,1);
    else
        Disp_Chn1616_Str(25,16,index_fault,1);
    
    //曝气机状态  
    if(Get_EngineStat(0) == DEVICE_DISABLE)
        Disp_Chn1616_Str(12,32,index_disable,1);
    else if(Get_EngineStat(0) == DEVICE_STOP)
        Disp_Chn1616_Str(12,32,index_stop,1); 
    else if(Get_EngineStat(0) == DEVICE_RUN)
        Disp_Chn1616_Str(12,32,index_run,1);
    else
        Disp_Chn1616_Str(12,32,index_fault,1);
    
    if(Get_EngineStat(1) == DEVICE_DISABLE)
        Disp_Chn1616_Str(25,32,index_disable,1);
    else if(Get_EngineStat(1) == DEVICE_STOP)
        Disp_Chn1616_Str(25,32,index_stop,1); 
    else if(Get_EngineStat(1) == DEVICE_RUN)
        Disp_Chn1616_Str(25,32,index_run,1);
    else
        Disp_Chn1616_Str(25,32,index_fault,1);
        
    //阀门状态    
    if(Get_ValueStat(0) == DEVICE_DISABLE)
        Disp_Chn1616_Str(12,48,index_disable,1); 
    else if(Get_ValueStat(0) == DEVICE_STOP)
        Disp_Chn1616_Str(12,48,index_close,1); 
    else if(Get_ValueStat(0) == DEVICE_RUN)
        Disp_Chn1616_Str(12,48,index_open,1);
	else
        Disp_Chn1616_Str(12,48,index_fault,1);
    
    if(Get_ValueStat(1) == DEVICE_DISABLE)
        Disp_Chn1616_Str(25,48,index_disable,1); 
    else if(Get_ValueStat(1) == DEVICE_STOP)
        Disp_Chn1616_Str(25,48,index_close,1); 
    else if(Get_ValueStat(1) == DEVICE_RUN)
        Disp_Chn1616_Str(25,48,index_open,1);
	else
        Disp_Chn1616_Str(25,48,index_fault,1);
    
    //回流泵状态  
    if(Get_BackflowStat(0) == DEVICE_DISABLE)
        Disp_Chn1616_Str(12,64,index_disable,1); 
    else if(Get_BackflowStat(0) == DEVICE_STOP)
        Disp_Chn1616_Str(12,64,index_stop,1); 
    else if(Get_BackflowStat(0) == DEVICE_RUN)
        Disp_Chn1616_Str(12,64,index_run,1);
    else
        Disp_Chn1616_Str(12,64,index_fault,1);
    
    if(Get_BackflowStat(1) == DEVICE_DISABLE)
        Disp_Chn1616_Str(25,64,index_disable,1); 
    else if(Get_BackflowStat(1) == DEVICE_STOP)
        Disp_Chn1616_Str(25,64,index_stop,1); 
    else if(Get_BackflowStat(1) == DEVICE_RUN)
        Disp_Chn1616_Str(25,64,index_run,1);
    else
        Disp_Chn1616_Str(25,64,index_fault,1);
	
	//回用泵状态  
    if(Get_ReuseStat(0) == DEVICE_DISABLE)
        Disp_Chn1616_Str(12,80,index_disable,1); 
    else if(Get_ReuseStat(0) == DEVICE_STOP)
        Disp_Chn1616_Str(12,80,index_stop,1); 
    else if(Get_ReuseStat(0) == DEVICE_RUN)
        Disp_Chn1616_Str(12,80,index_run,1);
    else
        Disp_Chn1616_Str(12,80,index_fault,1);
    
    if(Get_ReuseStat(1) == DEVICE_DISABLE)
        Disp_Chn1616_Str(25,80,index_disable,1); 
    else if(Get_ReuseStat(1) == DEVICE_STOP)
        Disp_Chn1616_Str(25,80,index_stop,1); 
    else if(Get_ReuseStat(1) == DEVICE_RUN)
        Disp_Chn1616_Str(25,80,index_run,1);
    else
        Disp_Chn1616_Str(25,80,index_fault,1);
    
    //水位状态  
	if(Get_LevelStat(0) == DEVICE_DISABLE)
        Disp_Chn1616_Char(14,96,105,1);
    else if(Get_LevelStat(0) == LEVEL_HIGH)
        Disp_Chn1616_Char(14,96,50,1); 
    else if(Get_LevelStat(0) == LEVEL_MID)
        Disp_Chn1616_Char(14,96,51,1);
    else
        Disp_Chn1616_Char(14,96,52,1);
	
	if(Get_LevelStat(3) == DEVICE_DISABLE)
        Disp_Chn1616_Char(27,96,105,1);
	else if(Get_LevelStat(3) == LEVEL_HIGH)
        Disp_Chn1616_Char(27,96,50,1); 
    else if(Get_LevelStat(3) == LEVEL_MID)
        Disp_Chn1616_Char(27,96,51,1);
    else
        Disp_Chn1616_Char(27,96,52,1);
	
	if(Get_LevelStat(1) == DEVICE_DISABLE)
        Disp_Chn1616_Char(14,112,105,1);
    else if(Get_LevelStat(1) == LEVEL_HIGH)
        Disp_Chn1616_Char(14,112,50,1); 
    else if(Get_LevelStat(1) == LEVEL_MID)
        Disp_Chn1616_Char(14,112,51,1);
    else
        Disp_Chn1616_Char(14,112,52,1);
	
	if(Get_LevelStat(2) == DEVICE_DISABLE)
        Disp_Chn1616_Char(27,112,105,1);
	else if(Get_LevelStat(2) == LEVEL_HIGH)
        Disp_Chn1616_Char(27,112,50,1); 
    else if(Get_LevelStat(2) == LEVEL_MID)
        Disp_Chn1616_Char(27,112,51,1);
    else
        Disp_Chn1616_Char(27,112,52,1);
}

//数据查看
void Dis_DataWatch(INT8U AllRefresh)  
{
    static INT8U index_stat[] = {
        19,20,0  
    };
    static INT8U index_data[] = {
        28,31,0  
    };
    static INT8U index_log[] = {
        34,19,35,0  
    };
    static INT8U index_pumpflow[] = {
        36,37,54,55,0  
    };
	static INT8U index_backflowflow[] = {
        123,124,54,55,0  
    };
	static INT8U index_reuseflow[] = {
        88,80,54,55,0  
    };
	static INT8U index_energy[] = {
        105,56,0  
    };
	static INT8U index_wind[] = {
        113,55,0  
    };
	static INT8U index_o2[] = {
        114,115,56,0  
    };
	static INT8U index_volt[] = {
        105,125,0  
    };
    
    float val;
    char i,j,len,num,flag;
    char str[32];
    
    if(AllRefresh)
    {
        Clr_LCDRam(); 
        Disp_Yline(4,0);
        
        Disp_Chn1616_Str(0,15,index_stat,1);
        Disp_Chn1616_Str(0,35,index_data,0);
        Disp_Chn816_Str(0,55,index_log,1);
    }
    
    if(!AllRefresh)
    {
     	
    }
	
	num = 0;
	for(i=0;i<8;++i)
	{
		flag = 0;
		if(memcmp(Ad_Param[i].id,"FU1",3) == 0)
		{
			//显示中文名称
			Disp_Chn1616_Str(5,2+18*num,index_pumpflow,1);
			Disp_Chn816_Char(13,2+18*num,14,1);
			//Clr_RamBlock(11,2+18*num,13-11);
			//获得数据
			val = Ad_RealData[i];
			flag = 1;
		}
		else if(memcmp(Ad_Param[i].id,"FU2",3) == 0)
		{
			//显示中文名称
			Disp_Chn1616_Str(5,2+18*num,index_backflowflow,1);
			Disp_Chn816_Char(13,2+18*num,14,1);
			//Clr_RamBlock(11,2+18*num,13-11);
			//获得数据
			val = Ad_RealData[i];
			flag = 1;
		}
		else if(memcmp(Ad_Param[i].id,"FU3",3) == 0)
		{
			//显示中文名称
			Disp_Chn1616_Str(5,2+18*num,index_reuseflow,1);
			Disp_Chn816_Char(13,2+18*num,14,1);
			//Clr_RamBlock(11,2+18*num,13-11);
			//获得数据
			val = Ad_RealData[i];
			flag = 1;
		}
		else if(memcmp(Ad_Param[i].id,"WU1",3) == 0)
		{
			//显示中文名称
			Disp_Chn816_Char(5,2+18*num,1,1);
			Disp_Chn816_Char(6,2+18*num,35,1);
			Disp_Chn1616_Str(7,2+18*num,index_wind,1);
			Disp_Chn816_Char(13,2+18*num,14,1);
			Clr_RamBlock(11,2+18*num,13-11);
			//获得数据
			val = Ad_RealData[i];
			flag = 1;
		}
		else if(memcmp(Ad_Param[i].id,"WU2",3) == 0)
		{
			//显示中文名称
			Disp_Chn816_Char(5,2+18*num,2,1);
			Disp_Chn816_Char(6,2+18*num,35,1);
			Disp_Chn1616_Str(7,2+18*num,index_wind,1);
			Disp_Chn816_Char(13,2+18*num,14,1);
			Clr_RamBlock(11,2+18*num,13-11);
			//获得数据
			val = Ad_RealData[i];
			flag = 1;
		}
		else if(memcmp(Ad_Param[i].id,"O21",3) == 0)
		{
			//显示中文名称
			Disp_Chn816_Char(5,2+18*num,1,1);
			Disp_Chn816_Char(6,2+18*num,35,1);
			Disp_Chn1616_Str(7,2+18*num,index_o2,1);
			Disp_Chn816_Char(13,2+18*num,14,1);
			//获得数据
			val = Ad_RealData[i];
			flag = 1;
		}
		else if(memcmp(Ad_Param[i].id,"O22",3) == 0)
		{
			//显示中文名称
			Disp_Chn816_Char(5,2+18*num,2,1);
			Disp_Chn816_Char(6,2+18*num,35,1);
			Disp_Chn1616_Str(7,2+18*num,index_o2,1);
			Disp_Chn816_Char(13,2+18*num,14,1);
			//获得数据
			val = Ad_RealData[i];
			flag = 1;
		}
		
		if(flag == 1)
		{
			//显示数据
			sprintf(str, "%.8f", val);
			len = (strlen(str) < 14) ? strlen(str) : 14;
			for(j=0;j<len;++j)
			{
				if(str[j] == '.')
					Disp_Chn816_Char(15+j,2+18*num,10,1); 
				else
					Disp_Chn816_Char(15+j,2+18*num,str[j]&0x0f,1);   
			}
			Clr_RamBlock(15+j,2+18*num,14-j);
			//显示设备数加1
			num++;
			if(num >= 7)
				break;
		}
	}
	for(i=0;i<8;++i)
	{
		if(num >= 7)
			break;
		
		flag = 0;
		if(memcmp(Com_Param.regid[i],"ECS",3) == 0)
		{
			//显示中文名称
			Disp_Chn1616_Str(5,2+18*num,index_energy,1);
			Disp_Chn816_Char(13,2+18*num,14,1);
			Clr_RamBlock(9,2+18*num,13-9);
			//获得数据
			val = Com_Val[i];
			flag = 1;
		}
		else if(memcmp(Com_Param.regid[i],"UAP",3) == 0)
		{
			//显示中文名称
			Disp_Chn1616_Str(5,2+18*num,index_volt,1);
			Disp_Chn816_Char(9,2+18*num,21,1);
			Disp_Chn816_Char(13,2+18*num,14,1);
			Clr_RamBlock(10,2+18*num,13-10);
			//获得数据
			val = Com_Val[i];
			flag = 1;
		}
		else if(memcmp(Com_Param.regid[i],"UBP",3) == 0)
		{
			//显示中文名称
			Disp_Chn1616_Str(5,2+18*num,index_volt,1);
			Disp_Chn816_Char(9,2+18*num,26,1);
			Disp_Chn816_Char(13,2+18*num,14,1);
			Clr_RamBlock(10,2+18*num,13-10);
			//获得数据
			val = Com_Val[i];
			flag = 1;
		}
		else if(memcmp(Com_Param.regid[i],"UCP",3) == 0)
		{
			//显示中文名称
			Disp_Chn1616_Str(5,2+18*num,index_volt,1);
			Disp_Chn816_Char(9,2+18*num,38,1);
			Disp_Chn816_Char(13,2+18*num,14,1);
			Clr_RamBlock(10,2+18*num,13-10);
			//获得数据
			val = Com_Val[i];
			flag = 1;
		}
				
		if(flag == 1)
		{
			//显示数据
			sprintf(str, "%.8f", val);
			len = (strlen(str) < 14) ? strlen(str) : 14;
			for(j=0;j<len;++j)
			{
				if(str[j] == '.')
					Disp_Chn816_Char(15+j,2+18*num,10,1); 
				else
					Disp_Chn816_Char(15+j,2+18*num,str[j]&0x0f,1);   
			}
			Clr_RamBlock(15+j,2+18*num,14-j);
			//显示设备数加1
			num++;
			if(num >= 7)
				return;
		}
	}
	
    for(i=num; i<7; ++i)
    	Clr_RamBlock(5,2+18*i,25); 
}

//LOG查看
void Dis_LogWatch(INT8U AllRefresh)  
{
    static INT8U index_stat[] = {
        19,20,0  
    };
    static INT8U index_data[] = {
        28,31,0  
    };
    static INT8U index_log[] = {
        34,19,35,0  
    };
    static INT8U index_starttime[] = {
        62,24,13,14,0  
    };
	static INT8U index_pwrstat[] = {
        126,105,19,20,0  
    };
	static INT8U index_pwron[] = {
        127,128,0  
    };
	static INT8U index_pwroff[] = {
        49,50,0  
    };
	static INT8U index_pwrunknown[] = {
        129,130,0  
    };
	    
    int i;
        
    if(AllRefresh)
    {
        Clr_LCDRam(); 
        Disp_Yline(4,0);
        
        Disp_Chn1616_Str(0,15,index_stat,1);
        Disp_Chn1616_Str(0,35,index_data,1);
        Disp_Chn816_Str(0,55,index_log,0);
                
        Disp_Chn1616_Str(5,5,index_starttime,1);
        Disp_Chn816_Char(13,5,14,1);
		
		Disp_Chn1616_Str(5,25,index_pwrstat,1);
        Disp_Chn816_Char(13,25,14,1);
    }
    
    if(!AllRefresh)
    {
        
    }
    
    for(i=0;i<strlen((char *)System_StartTimeStr);++i)
        Disp_Chn816_Char(15+i,5,System_StartTimeStr[i]&0x0f,1); 

	if(PWR_Stat == DEVICE_STOP)
	{
		Disp_Chn1616_Str(15,25,index_pwroff,1);
	}
	else if(PWR_Stat == DEVICE_RUN)
	{
		Disp_Chn1616_Str(15,25,index_pwron,1);
	}
	else
	{
		Disp_Chn1616_Str(15,25,index_pwrunknown,1);
	}
}

//用户密码
void Dis_PWInput(INT8U AllRefresh)            
{
	static INT8U index_pw[] = {
        57,58,59,60,61,0  
    };
    
    char i;
    
    if(AllRefresh)
    {
        Clr_LCDRam();
        Disp_Chn1616_Str(10,40,index_pw,1);
        Disp_Chn816_Char(10,64,12,0);
        memset(Input_Temp, '0', sizeof(Input_Temp));
        Input_Pos = 0;
    }
    else
    {
        //当前输入位置显示实际字符，其它位置显示星号
        Clr_RamBlock(10,64,6);
        for(i=0;i<Input_Pos;++i)   
           Disp_Chn816_Char(10+i,64,12,1); 
        Disp_Chn816_Char(10+Input_Pos,64,Input_Temp[Input_Pos]&0x0f,0); 
    }
}

//设备控制
void Dis_Control(INT8U AllRefresh)            
{
    static INT8U index_pump[] = {
        36,37,0  
    };
    static INT8U index_engine[] = {
        39,40,0  
    };
    static INT8U index_value[] = {
        103,104,0  
    };
	static INT8U index_backflow[] = {
        123,124,0  
    };
	static INT8U index_reuse[] = {
        88,80,0  
    };
	
    static INT8U index_stop[] = {
        45,46,0  
    };
    static INT8U index_start[] = {
        62,24,0  
    };
    static INT8U index_disable[] = {
        106,4,0  
    };
	static INT8U index_open[] = {
        119,62,0  
    };
    static INT8U index_close[] = {
        120,121,0  
    };
        
    
    char pon[16];
    
    if(AllRefresh)
    {
        SndDis_Index = 0;
        
        Clr_LCDRam(); 
                
        Disp_Yline(14,0);
		
		Disp_Chn816_Char(1,2,1,1);
        Disp_Chn816_Char(2,2,35,1);
        Disp_Chn1616_Str(3,2,index_pump,1);
        Disp_Chn816_Char(7,2,14,1);
		Disp_Chn816_Char(16,2,2,1);
        Disp_Chn816_Char(17,2,35,1);
        Disp_Chn1616_Str(18,2,index_pump,1);
        Disp_Chn816_Char(22,2,14,1);
		
		Disp_Chn816_Char(1,20,1,1);
        Disp_Chn816_Char(2,20,35,1);
        Disp_Chn1616_Str(3,20,index_engine,1);
        Disp_Chn816_Char(7,20,14,1);
		Disp_Chn816_Char(16,20,2,1);
        Disp_Chn816_Char(17,20,35,1);
        Disp_Chn1616_Str(18,20,index_engine,1);
        Disp_Chn816_Char(22,20,14,1);
		
        Disp_Chn816_Char(1,38,1,1);
        Disp_Chn816_Char(2,38,35,1);
        Disp_Chn1616_Str(3,38,index_value,1);
        Disp_Chn816_Char(7,38,14,1);
        Disp_Chn816_Char(16,38,2,1);
        Disp_Chn816_Char(17,38,35,1);
        Disp_Chn1616_Str(18,38,index_value,1);
        Disp_Chn816_Char(22,38,14,1);
        
        Disp_Chn816_Char(1,56,1,1);
        Disp_Chn816_Char(2,56,35,1);
        Disp_Chn1616_Str(3,56,index_backflow,1);
        Disp_Chn816_Char(7,56,14,1);
		Disp_Chn816_Char(16,56,2,1);
        Disp_Chn816_Char(17,56,35,1);
        Disp_Chn1616_Str(18,56,index_backflow,1);
        Disp_Chn816_Char(22,56,14,1);
		
		Disp_Chn816_Char(1,74,1,1);
        Disp_Chn816_Char(2,74,35,1);
        Disp_Chn1616_Str(3,74,index_reuse,1);
        Disp_Chn816_Char(7,74,14,1);
		Disp_Chn816_Char(16,74,2,1);
        Disp_Chn816_Char(17,74,35,1);
        Disp_Chn1616_Str(18,74,index_reuse,1);
        Disp_Chn816_Char(22,74,14,1);
	}
    
    if(!AllRefresh)
    {
    }
    
    memset(pon,1,sizeof(pon)); 
    if(Get_WorkType() != WORKTYPE_AUTO)
        pon[SndDis_Index] = 0;
	
	//提升泵
	if(Get_PumpControlStat(0) == DEVICE_DISABLE)
        Disp_Chn1616_Str(9,2,index_disable,pon[0]); 
    else if(Get_PumpControlStat(0) == DEVICE_RUN)
        Disp_Chn1616_Str(9,2,index_stop,pon[0]);
    else
        Disp_Chn1616_Str(9,2,index_start,pon[0]);
	
    if(Get_PumpControlStat(1) == DEVICE_DISABLE)
        Disp_Chn1616_Str(24,2,index_disable,pon[1]); 
    else if(Get_PumpControlStat(1) == DEVICE_RUN)
        Disp_Chn1616_Str(24,2,index_stop,pon[1]);
    else
        Disp_Chn1616_Str(24,2,index_start,pon[1]);
	
	//曝气机
	if(Get_EngineControlStat(0) == DEVICE_DISABLE)
        Disp_Chn1616_Str(9,20,index_disable,pon[2]); 
    else if(Get_EngineControlStat(0) == DEVICE_RUN)
        Disp_Chn1616_Str(9,20,index_stop,pon[2]);
    else
        Disp_Chn1616_Str(9,20,index_start,pon[2]);
            
    if(Get_EngineControlStat(1) == DEVICE_DISABLE)
        Disp_Chn1616_Str(24,20,index_disable,pon[3]); 
    else if(Get_EngineControlStat(1) == DEVICE_RUN)
        Disp_Chn1616_Str(24,20,index_stop,pon[3]);
    else
        Disp_Chn1616_Str(24,20,index_start,pon[3]);
	
    //电磁阀
    if(Get_ValueControlStat(0) == DEVICE_DISABLE)
        Disp_Chn1616_Str(9,38,index_disable,pon[4]); 
    else if(Get_ValueControlStat(0) == DEVICE_RUN)
        Disp_Chn1616_Str(9,38,index_close,pon[4]);
    else
        Disp_Chn1616_Str(9,38,index_open,pon[4]);
	
    if(Get_ValueControlStat(1) == DEVICE_DISABLE)
        Disp_Chn1616_Str(24,38,index_disable,pon[5]); 
    else if(Get_ValueControlStat(1) == DEVICE_RUN)
        Disp_Chn1616_Str(24,38,index_close,pon[5]);
    else
        Disp_Chn1616_Str(24,38,index_open,pon[5]);
            
    //回流泵
	if(Get_BackflowControlStat(0) == DEVICE_DISABLE)
        Disp_Chn1616_Str(9,56,index_disable,pon[6]); 
    else if(Get_BackflowControlStat(0) == DEVICE_RUN)
        Disp_Chn1616_Str(9,56,index_stop,pon[6]);
    else
        Disp_Chn1616_Str(9,56,index_start,pon[6]);
            
    if(Get_BackflowControlStat(1) == DEVICE_DISABLE)
        Disp_Chn1616_Str(24,56,index_disable,pon[7]); 
    else if(Get_BackflowControlStat(1) == DEVICE_RUN)
        Disp_Chn1616_Str(24,56,index_stop,pon[7]);
    else
        Disp_Chn1616_Str(24,56,index_start,pon[7]);
	
	//回用泵
    if(Get_ReuseControlStat(0) == DEVICE_DISABLE)
        Disp_Chn1616_Str(9,74,index_disable,pon[8]); 
    else if(Get_ReuseControlStat(0) == DEVICE_RUN)
        Disp_Chn1616_Str(9,74,index_stop,pon[8]);
    else
        Disp_Chn1616_Str(9,74,index_start,pon[8]);
            
    if(Get_ReuseControlStat(1) == DEVICE_DISABLE)
        Disp_Chn1616_Str(24,74,index_disable,pon[9]); 
    else if(Get_ReuseControlStat(1) == DEVICE_RUN)
        Disp_Chn1616_Str(24,74,index_stop,pon[9]);
    else
        Disp_Chn1616_Str(24,74,index_start,pon[9]);
    
}

//设置选择
void Dis_Setup(INT8U AllRefresh)    
{
    static INT8U index_work[] = {
        32,101,27,28,0  
    };
    static INT8U index_ai[] = {
        22,20,0  
    };
    static INT8U index_setup[] = {
        29,30,0  
    }; 
    static INT8U index_system[] = {
        63,64,27,28,0  
    };
    static INT8U index_device[] = {
        29,95,27,28,0  
    };
    static INT8U index_communication[] = {
        65,66,27,28,0  
    };
    char pon[8];
    
    if(AllRefresh)
    {
        Clr_LCDRam(); 
        SndDis_Index = 0;
    }
    
    memset(pon,1,sizeof(pon));
    pon[SndDis_Index] = 0;
    Disp_Chn1616_Str(2,40,index_work,pon[0]);
    Disp_Chn1616_Str(12,40,index_device,pon[1]);
    Disp_Chn816_Str(22,40,index_ai,pon[2]);
    Disp_Chn1616_Str(24,40,index_setup,pon[2]);
    Disp_Chn1616_Str(5,72,index_system,pon[3]);
    Disp_Chn1616_Str(17,72,index_communication,pon[4]);
}

//AI设置
void Dis_AISetup(INT8U AllRefresh)    
{
    static INT8U index_ai[] = {
        22,20,0  
    };
    static INT8U index_type[] = {
        67,68,69,70,0  
    };
    static INT8U index_code[] = {
        67,68,71,61,0  
    };
    static INT8U index_highval[] = {
        56,72,73,75,0  
    };
    static INT8U index_lowval[] = {
        56,72,74,75,0  
    };
    static INT8U index_signal[][8] = {
        {5,14,3,1,23,22,0},
        {1,14,6,16,0},
        {1,14,2,1,16,0}
    };
    
    char i,len,t;
    char pon[8],str[24];
    
    if(AllRefresh)
    {
        Clr_LCDRam();
        Input_Type = 0;
        SndDis_Index = 0;
        ThdDis_Index = 0;
        Input_Pos = 0;
    }
    
    //当前的通道号高亮显示，其它通道正常显示
    memset(pon,1,sizeof(pon));
    pon[SndDis_Index] = 0;
    for(i=0;i<8;++i)
    {
        Disp_Chn816_Str(1,16*i,index_ai,pon[i]);   
        Disp_Chn816_Char(3,16*i,i+1,pon[i]);
    }
    Disp_Yline(5,0); 
    
    //当前的设置项高亮显示，其它通道正常显示
    memset(pon,1,sizeof(pon));
    if(Input_Type > 0)
        pon[ThdDis_Index] = 0;
    
    Disp_Chn1616_Str(7,5,index_type,pon[0]);
    Disp_Chn1616_Str(7,25,index_code,pon[1]);
    Disp_Chn1616_Str(7,45,index_highval,pon[2]);
    Disp_Chn1616_Str(7,65,index_lowval,pon[3]);
     
    Clr_RamBlock(17,5,6);
    Clr_RamBlock(17,25,3);
    Clr_RamBlock(17,45,10);
    Clr_RamBlock(17,65,10);
    
    //输入类型
    if((Input_Type == 2) && (ThdDis_Index == 0))
        Disp_Chn816_Str(17,5,index_signal[Input_Temp[0]],0); 
    else
        Disp_Chn816_Str(17,5,index_signal[Ad_Param[SndDis_Index].type],1);
    
    //代码
    if((Input_Type == 2) && (ThdDis_Index == 1))
    {
        for(i=0;i<3;++i)
        {
            char t;
            if(i == Input_Pos)
                t = 0; 
            else
                t = 1;
            
            if(Input_Temp[i] == 'F')
                Disp_Chn816_Char(17+i,25,25,t); 
            else if(Input_Temp[i] == 'U')
                Disp_Chn816_Char(17+i,25,30,t); 
			else if(Input_Temp[i] == 'W')
                Disp_Chn816_Char(17+i,25,41,t); 
			else if(Input_Temp[i] == 'O')
                Disp_Chn816_Char(17+i,25,18,t); 
			else
                Disp_Chn816_Char(17+i,25,Input_Temp[i]&0x0f,t); 
        }
    }
    else
    {
        for(i=0;i<3;++i)
        {
            if(Ad_Param[SndDis_Index].id[i] == 'F')
                Disp_Chn816_Char(17+i,25,25,1); 
            else if(Ad_Param[SndDis_Index].id[i] == 'U')
                Disp_Chn816_Char(17+i,25,30,1);
			else if(Ad_Param[SndDis_Index].id[i] == 'W')
                Disp_Chn816_Char(17+i,25,41,1);
			else if(Ad_Param[SndDis_Index].id[i] == 'O')
                Disp_Chn816_Char(17+i,25,18,1);
			else
                Disp_Chn816_Char(17+i,25,Ad_Param[SndDis_Index].id[i]&0x0f,1); 
        }
    }
    
    //上限
    if((Input_Type == 2) && (ThdDis_Index == 2))
    {
        for(i=0;i<=Input_Pos;++i)
        {
            if(i == Input_Pos)
                t = 0;
            else
                t = 1;
            
            if(Input_Temp[i] == '.')
                Disp_Chn816_Char(17+i,45,10,t); 
            else
                Disp_Chn816_Char(17+i,45,Input_Temp[i]&0x0f,t);   
        }
    }
    else
    {
        sprintf(str, "%.8f", Ad_Param[SndDis_Index].highval);
        len = (strlen(str) < 10) ? strlen(str) : 10;
        for(i=0;i<len;++i)
        {
            if(str[i] == '.')
                Disp_Chn816_Char(17+i,45,10,1); 
            else
                Disp_Chn816_Char(17+i,45,str[i]&0x0f,1);   
        }
    }
    
    //下限
    if((Input_Type == 2) && (ThdDis_Index == 3))
    {
        for(i=0;i<=Input_Pos;++i)
        {
            if(i == Input_Pos)
                t = 0;
            else
                t = 1;
            
            if(Input_Temp[i] == '.')
                Disp_Chn816_Char(17+i,65,10,t); 
            else
                Disp_Chn816_Char(17+i,65,Input_Temp[i]&0x0f,t);   
        }
    }
    else
    {
        sprintf(str, "%.8f", Ad_Param[SndDis_Index].lowval);
        len = (strlen(str) < 10) ? strlen(str) : 10;
        for(i=0;i<len;++i)
        {
            if(str[i] == '.')
                Disp_Chn816_Char(17+i,65,10,1); 
            else
                Disp_Chn816_Char(17+i,65,str[i]&0x0f,1);   
        }
    }
}

//工作参数
void Dis_WorkSetup(INT8U AllRefresh)    
{
    static INT8U index_remotecontrol[] = {
        100,72,23,24,25,26,0  
    };
	static INT8U index_dohigh[] = {
        39,40,45,46,114,115,56,83,0  
    };
	static INT8U index_dolow[] = {
        39,40,47,48,114,115,56,83,0  
    };
	static INT8U index_engineswitchinterval[] = {
        39,40,116,117,14,87,0  
    };
	static INT8U index_valueruninterval[] = {
        103,104,47,48,14,87,0  
    };
	static INT8U index_valueruntime[] = {
        103,104,47,48,13,14,0  
    };
	static INT8U index_open[] = {
        119,62,0  
    };
    static INT8U index_close[] = {
        120,121,0  
    };
            
    char i;
    char pon[8],str[24];
    int len;
    
    if(AllRefresh)
    {
        Clr_LCDRam();
        Input_Type = 0;
        SndDis_Index = 0;
        Input_Pos = 0;
    }
    
    memset(pon,1,sizeof(pon));
    pon[SndDis_Index] = 0;
    
    Disp_Chn1616_Str(0,2,index_remotecontrol,pon[0]);
	Disp_Chn1616_Str(0,20,index_dohigh,pon[1]);
	Disp_Chn1616_Str(0,38,index_dolow,pon[2]);
    Disp_Chn1616_Str(0,56,index_engineswitchinterval,pon[3]);
	Disp_Chn1616_Str(0,74,index_valueruninterval,pon[4]);
	Disp_Chn1616_Str(0,92,index_valueruntime,pon[5]);
        
    Disp_Yline(16,0); 
    
   	Clr_RamBlock(18,20,10);
	Clr_RamBlock(18,38,10);
	Clr_RamBlock(18,56,5);
	Clr_RamBlock(18,74,5);
	Clr_RamBlock(18,92,5);
    
	//远程控制
	if((Input_Type == 1) && (SndDis_Index == 0))
    {
        if(Input_Temp[0] == WORKTYPE_REMOTE)
            Disp_Chn1616_Str(18,2,index_open,0); 
        else
            Disp_Chn1616_Str(18,2,index_close,0); 
    }
    else
    {
        if(Sys_Param.worktype == WORKTYPE_REMOTE)
            Disp_Chn1616_Str(18,2,index_open,1); 
        else
            Disp_Chn1616_Str(18,2,index_close,1);  
    }
        
    //do高值
    if((Input_Type == 1) && (SndDis_Index == 1))
    {
        for(i=0;i<=Input_Pos;++i)
        {
            INT8U t;
            
            if(i == Input_Pos)
                t = 0;
            else
                t = 1;
            
            if(Input_Temp[i] == '.')
                Disp_Chn816_Char(18+i,20,10,t); 
            else
                Disp_Chn816_Char(18+i,20,Input_Temp[i]&0x0f,t);   
        }
    }
    else
    {
        sprintf(str, "%.4f", Sys_Param.dohigh);
        len = (strlen(str) < 10) ? strlen(str) : 10;
        for(i=0;i<len;++i)
        {
            if(str[i] == '.')
                Disp_Chn816_Char(18+i,20,10,1); 
            else
                Disp_Chn816_Char(18+i,20,str[i]&0x0f,1);   
        } 
    }
	
	//do低值
    if((Input_Type == 1) && (SndDis_Index == 2))
    {
        for(i=0;i<=Input_Pos;++i)
        {
            INT8U t;
            
            if(i == Input_Pos)
                t = 0;
            else
                t = 1;
            
            if(Input_Temp[i] == '.')
                Disp_Chn816_Char(18+i,38,10,t); 
            else
                Disp_Chn816_Char(18+i,38,Input_Temp[i]&0x0f,t);   
        }
    }
    else
    {
        sprintf(str, "%.4f", Sys_Param.dolow);
        len = (strlen(str) < 10) ? strlen(str) : 10;
        for(i=0;i<len;++i)
        {
            if(str[i] == '.')
                Disp_Chn816_Char(18+i,38,10,1); 
            else
                Disp_Chn816_Char(18+i,38,str[i]&0x0f,1);   
        } 
    }
    
    //曝气机切换的间隔时间
    if((Input_Type == 1) && (SndDis_Index == 3))
    {
        for(i=0;i<=Input_Pos;++i)
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(18+i,56,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(18+i,56,Input_Temp[i]&0x0f,1);
        }
    }
    else
    {
        sprintf(str,"%d",Sys_Param.engine_switchlong);
        for(i=0;i<strlen(str);++i)
            Disp_Chn816_Char(18+i,56,str[i]&0x0f,1); 
    }
    
    //阀门运行切换的间隔时间
    if((Input_Type == 1) && (SndDis_Index == 4))
    {
        for(i=0;i<=Input_Pos;++i)
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(18+i,74,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(18+i,74,Input_Temp[i]&0x0f,1);
        }
    }
    else
    {
        sprintf(str,"%d",Sys_Param.value_switchlong);
        for(i=0;i<strlen(str);++i)
            Disp_Chn816_Char(18+i,74,str[i]&0x0f,1); 
    }
    
    //阀门的运行时长
    if((Input_Type == 1) && (SndDis_Index == 5))
    {
        for(i=0;i<=Input_Pos;++i)
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(18+i,92,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(18+i,92,Input_Temp[i]&0x0f,1);
        }
    }
    else
    {
        sprintf(str,"%d",Sys_Param.value_switchlast);
        for(i=0;i<strlen(str);++i)
            Disp_Chn816_Char(18+i,92,str[i]&0x0f,1); 
    }
}

//设备参数
void Dis_DevSetup(INT8U AllRefresh)
{
    static INT8U index_pump[] = {
        36,37,38,0  
    };
    static INT8U index_engine[] = {
        39,40,41,0  
    };
    static INT8U index_value[] = {
        123,124,103,0  
    };
	static INT8U index_backflow[] = {
        123,124,38,0  
    };
	static INT8U index_reuse[] = {
        88,80,38,0  
    };
    static INT8U index_enable[] = {
        62,4,0  
    };
	static INT8U index_pumplevel[] = {
        36,37,42,43,0  
    };
	static INT8U index_backflowlevel[] = {
        124,42,43,0  
    };
	static INT8U index_reuselevel[] = {
        88,80,42,43,0  
    };
    static INT8U index_disable[] = {
        106,4,0  
    };
    
    char i,pon[16];
       
    if(AllRefresh)
    {
        Clr_LCDRam();
        Input_Type = 0;
        SndDis_Index = 0;
        Input_Pos = 0;
    }
    
    memset(pon,1,sizeof(pon));
    pon[SndDis_Index] = 0;
    
	//第一列
    for(i=0;i<2;++i)
    {
        Disp_Chn816_Char(0,16*i,i+1,pon[i]);
        Disp_Chn816_Char(1,16*i,35,pon[i]);
        Disp_Chn1616_Str(2,16*i,index_pump,pon[i]); 
    }
    for(i=0;i<2;++i)
    {
        Disp_Chn816_Char(0,16*(2+i),i+1,pon[2+i]);
        Disp_Chn816_Char(1,16*(2+i),35,pon[2+i]);
        Disp_Chn1616_Str(2,16*(2+i),index_engine,pon[2+i]); 
    }
    for(i=0;i<2;++i)
    {
        Disp_Chn816_Char(0,16*(4+i),i+1,pon[4+i]);
        Disp_Chn816_Char(1,16*(4+i),35,pon[4+i]);
        Disp_Chn1616_Str(2,16*(4+i),index_value,pon[4+i]); 
    }
	for(i=0;i<2;++i)
    {
        Disp_Chn816_Char(0,16*(6+i),i+1,pon[6+i]);
        Disp_Chn816_Char(1,16*(6+i),35,pon[6+i]);
        Disp_Chn1616_Str(2,16*(6+i),index_backflow,pon[6+i]); 
    }
	Disp_Yline(8,0); 
	
	//第二列
	for(i=0;i<2;++i)
    {
        Disp_Chn816_Char(16,16*i,i+1,pon[8+i]);
        Disp_Chn816_Char(17,16*i,35,pon[8+i]);
        Disp_Chn1616_Str(18,16*i,index_reuse,pon[8+i]); 
    }
	Disp_Chn1616_Str(16,32,index_pumplevel,pon[10]);
	Disp_Chn816_Char(16,48,1,pon[11]);
    Disp_Chn816_Char(17,48,35,pon[11]);
	Disp_Chn1616_Str(18,48,index_backflowlevel,pon[11]); 
	Disp_Chn816_Char(16,64,2,pon[12]);
    Disp_Chn816_Char(17,64,35,pon[12]);
	Disp_Chn1616_Str(18,64,index_backflowlevel,pon[12]); 
	Disp_Chn1616_Str(16,80,index_reuselevel,pon[13]); 
    
    Disp_Yline(24,0); 
    
	//第一列
    for(i=0;i<2;++i)
    {
        if((Input_Type == 1) && (SndDis_Index == i))
        {
            if(Input_Temp[0] == 0)
                Disp_Chn1616_Str(9,16*i,index_disable,0); 
            else
                Disp_Chn1616_Str(9,16*i,index_enable,0); 
        }
        else
        {
            if((Sys_Param.pumpen&(1<<i)) == 0)
                Disp_Chn1616_Str(9,16*i,index_disable,1); 
            else
                Disp_Chn1616_Str(9,16*i,index_enable,1); 
        }
    }
	for(i=0;i<2;++i)
    {
        if((Input_Type == 1) && (SndDis_Index == 2+i))
        {
            if(Input_Temp[0] == 0)
                Disp_Chn1616_Str(9,16*(i+2),index_disable,0); 
            else
                Disp_Chn1616_Str(9,16*(i+2),index_enable,0); 
        }
        else
        {
            if((Sys_Param.engineen&(1<<i)) == 0)
                Disp_Chn1616_Str(9,16*(i+2),index_disable,1); 
            else
                Disp_Chn1616_Str(9,16*(i+2),index_enable,1); 
        }
    }
    for(i=0;i<2;++i)
    {
        if((Input_Type == 1) && (SndDis_Index == 4+i))
        {
            if(Input_Temp[0] == 0)
                Disp_Chn1616_Str(9,16*(i+4),index_disable,0); 
            else
                Disp_Chn1616_Str(9,16*(i+4),index_enable,0); 
        }
        else
        {
            if((Sys_Param.valueen&(1<<i)) == 0)
                Disp_Chn1616_Str(9,16*(i+4),index_disable,1); 
            else
                Disp_Chn1616_Str(9,16*(i+4),index_enable,1); 
        }
    }
	for(i=0;i<2;++i)
    {
        if((Input_Type == 1) && (SndDis_Index == 6+i))
        {
            if(Input_Temp[0] == 0)
                Disp_Chn1616_Str(9,16*(i+6),index_disable,0); 
            else
                Disp_Chn1616_Str(9,16*(i+6),index_enable,0); 
        }
        else
        {
            if((Sys_Param.backflowen&(1<<i)) == 0)
                Disp_Chn1616_Str(9,16*(i+6),index_disable,1); 
            else
                Disp_Chn1616_Str(9,16*(i+6),index_enable,1); 
        }
    }
	
	//第二列
	for(i=0;i<2;++i)
    {
        if((Input_Type == 1) && (SndDis_Index == 8+i))
        {
            if(Input_Temp[0] == 0)
                Disp_Chn1616_Str(25,16*i,index_disable,0); 
            else
                Disp_Chn1616_Str(25,16*i,index_enable,0); 
        }
        else
        {
            if((Sys_Param.reuseen&(1<<i)) == 0)
                Disp_Chn1616_Str(25,16*i,index_disable,1); 
            else
                Disp_Chn1616_Str(25,16*i,index_enable,1); 
        }
    }
	for(i=0;i<4;++i)
    {
        if((Input_Type == 1) && (SndDis_Index == 10+i))
        {
            if(Input_Temp[0] == 0)
                Disp_Chn1616_Str(25,16*(i+2),index_disable,0); 
            else
                Disp_Chn1616_Str(25,16*(i+2),index_enable,0); 
        }
        else
        {
            if((Sys_Param.levelen&(1<<i)) == 0)
                Disp_Chn1616_Str(25,16*(i+2),index_disable,1); 
            else
                Disp_Chn1616_Str(25,16*(i+2),index_enable,1); 
        }
    }
}

//系统参数
void Dis_SysSetup(INT8U AllRefresh)    
{
    static INT8U index_pw[] = {
        63,64,60,61,0  
    };
    static INT8U index_time[] = {
        63,64,13,14,0  
    };
    static INT8U index_sim[] = {
        17,20,38,0  
    };
    static INT8U index_num[] = {
        99,68,0  
    };
    static INT8U index_mn[] = {
        63,64,84,85,0  
    };
    static INT8U index_st[] = {
        63,64,71,68,0  
    };
    static INT8U index_rtd[] = {
        73,86,14,87,0  
    };
        
    char i;
    char pon[8];
    
    if(AllRefresh)
    {
        Clr_LCDRam();
        Input_Type = 0;
        SndDis_Index = 0;
        Input_Pos = 0;
    }
    
    memset(pon,1,sizeof(pon));
    pon[SndDis_Index] = 0;
    Disp_Chn1616_Str(1,5,index_pw,pon[0]);
    Disp_Chn1616_Str(1,25,index_time,pon[1]);
    Disp_Chn816_Str(1,45,index_sim,pon[2]);
    Disp_Chn1616_Str(4,45,index_num,pon[2]);
    Disp_Chn1616_Str(1,65,index_mn,pon[3]);
    Disp_Chn1616_Str(1,85,index_st,pon[4]);
    Disp_Chn1616_Str(1,105,index_rtd,pon[5]);
    
    Disp_Yline(10,0); 
    
    Clr_RamBlock(12,5,6);
    //Clr_RamBlock(12,25,14);
    //Clr_RamBlock(12,45,14);
    //Clr_RamBlock(12,65,2);
    //Clr_RamBlock(12,65,3);
    
    //密码
    if((Input_Type == 1) && (SndDis_Index == 0))
    {
        for(i=0;i<=Input_Pos;++i)
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(12+i,5,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(12+i,5,12,1);
        }
    }
    else
    {
        for(i=0;i<6;++i)
            Disp_Chn816_Char(12+i,5,12,1);
    }
    
    //系统时间
    if((Input_Type == 1) && (SndDis_Index == 1))
    {
        for(i=0;i<14;++i)
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(12+i,25,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(12+i,25,Input_Temp[i]&0x0f,1);
        }
    }
    else
    {
        for(i=0;i<14;++i)
            Disp_Chn816_Char(12+i,25,System_TimeStr[i]&0x0f,1); 
    }
    
    //SIM卡号
    if((Input_Type == 1) && (SndDis_Index == 2))
    {
        for(i=0;i<11;++i)
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(12+i,45,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(12+i,45,Input_Temp[i]&0x0f,1);
        }
    }
    else
    {
        for(i=0;i<11;++i)
            Disp_Chn816_Char(12+i,45,Sys_Param.sim[i]&0x0f,1); 
    }
    
    //MN号
    if((Input_Type == 1) && (SndDis_Index == 3))
    {
        for(i=0;i<14;++i)
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(12+i,65,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(12+i,65,Input_Temp[i]&0x0f,1);
        }
    }
    else
    {
        for(i=0;i<14;++i)
            Disp_Chn816_Char(12+i,65,Sys_Param.mn[i]&0x0f,1); 
    }
    
    //ST编码
    if((Input_Type == 1) && (SndDis_Index == 4))
    {
        for(i=0;i<2;++i)
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(12+i,85,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(12+i,85,Input_Temp[i]&0x0f,1);
        }
    }
    else
    {
        for(i=0;i<2;++i)
            Disp_Chn816_Char(12+i,85,Sys_Param.st[i]&0x0f,1); 
    }
    
    //实时数据发送间隔
    if((Input_Type == 1) && (SndDis_Index == 5))
    {
        for(i=0;i<3;++i)
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(12+i,105,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(12+i,105,Input_Temp[i]&0x0f,1);
        }
    }
    else
    {
        for(i=0;i<3;++i)
            Disp_Chn816_Char(12+i,105,Sys_Param.rtd[i]&0x0f,1); 
    }
}

//通信参数
/*void Dis_ComSetup(INT8U AllRefresh)    
{
    static INT8U index_rate[] = {
        92,93,94,0  
    };
    static INT8U index_code[] = {
        29,95,71,68,0  
    };
    static INT8U index_var[] = {
        96,56,0  
    };
    static INT8U index_addr[] = {
        97,98,0  
    };
    
        
    char i,regindex;
    char pon[10],str[6];
    
    if(AllRefresh)
    {
        Clr_LCDRam();
        Input_Type = 0;
        SndDis_Index = 0;
        Input_Pos = 0;
        
        Disp_Xline(25);   
    }
    
    memset(pon,1,sizeof(pon));
    pon[SndDis_Index] = 0;
    
    Disp_Chn1616_Str(1,5,index_rate,pon[0]);
    Disp_Chn1616_Str(16,5,index_code,pon[1]);
    
    for(i=0;i<MODBUS_VARNUM;++i)
    {
        Disp_Chn1616_Str(1,30+20*i,index_var,pon[2+i]); 
        Disp_Chn816_Char(5,30+20*i,i+1,pon[2+i]);
        Disp_Chn1616_Str(6,30+20*i,index_addr,pon[2+i]);
    }
         
    Clr_RamBlock(8,5,5);
    Clr_RamBlock(25,5,3);
    for(i=0;i<MODBUS_VARNUM;++i)
        Clr_RamBlock(12,30+20*i,6);
    
    //波特率
    if((Input_Type == 1) && (SndDis_Index == 0))
    {
        for(i=0;i<strlen(ComBaud_Str[Input_Temp[0]]);++i)
        {
            Disp_Chn816_Char(8+i,5,ComBaud_Str[Input_Temp[0]][i]&0x0f,0);
        }
    }
    else
    {
        for(i=0;i<strlen(ComBaud_Str[Com_Param.baud_index]);++i)
        {
            Disp_Chn816_Char(8+i,5,ComBaud_Str[Com_Param.baud_index][i]&0x0f,1);
        }
    }
    
    //设备编号
    if((Input_Type == 1) && (SndDis_Index == 1))
    {
        for(i=0;i<=Input_Pos;++i)
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(25+i,5,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(25+i,5,Input_Temp[i]&0x0f,1);
        }
    }
    else
    {
        sprintf(str,"%d",Com_Param.devid);
        for(i=0;i<strlen(str);++i)
            Disp_Chn816_Char(25+i,5,str[i]&0x0f,1); 
    }
    
    //变量地址
    for(regindex=0;regindex<MODBUS_VARNUM;++regindex)
    {
        if((Input_Type == 1) && (SndDis_Index == 2+regindex))
        {
            for(i=0;i<=Input_Pos;++i)
            {
                if(i == Input_Pos)
                    Disp_Chn816_Char(12+i,30+20*regindex,Input_Temp[i]&0x0f,0);
                else
                    Disp_Chn816_Char(12+i,30+20*regindex,Input_Temp[i]&0x0f,1);
            }
        }
        else
        {
            INT32U temp = Com_Param.regaddr[regindex];
            sprintf(str,"%ld",temp);
            for(i=0;i<strlen(str);++i)
                Disp_Chn816_Char(12+i,30+20*regindex,str[i]&0x0f,1); 
        }   
    }
}*/

//通信参数
void Dis_ComSetup(INT8U AllRefresh)    
{
    static INT8U index_rate[] = {
        92,93,94,0  
    };
    static INT8U index_var[] = {
        96,56,0  
    };
	static INT8U index_id[] = {
        28,31,71,61,0  
    };
	static INT8U index_code[] = {
        29,95,71,68,0  
    };
    static INT8U index_addr[] = {
        28,31,97,98,0  
    };
    
    char i;
    char pon[12],str[6];
    
    if(AllRefresh)
    {
        Clr_LCDRam();
        Input_Type = 0;
        SndDis_Index = 0;
        ThdDis_Index = 0;
        Input_Pos = 0;
        Disp_Xline(16);   
        Disp_Yline(7,16);   
    }
    
    memset(pon,1,sizeof(pon));
    pon[SndDis_Index] = 0;
    
    Disp_Chn1616_Str(11,0,index_rate,pon[0]);
        
    for(i=0;i<MODBUS_VARNUM;++i)
    {
        Disp_Chn1616_Str(1,16+16*i,index_var,pon[1+i]); 
        Disp_Chn816_Char(5,16+16*i,i+1,pon[1+i]);
    }
      
    Clr_RamBlock(18,0,5);
    Clr_RamBlock(9,30,8);
    Clr_RamBlock(19,30,3);
    Clr_RamBlock(9,50,8);
    Clr_RamBlock(19,50,3);
    Clr_RamBlock(9,70,8);
    Clr_RamBlock(19,70,5);
    
    //波特率
    if((Input_Type == 1) && (SndDis_Index == 0))
    {
        for(i=0;i<strlen(ComBaud_Str[Input_Temp[0]]);++i)
        {
            Disp_Chn816_Char(18+i,0,ComBaud_Str[Input_Temp[0]][i]&0x0f,0);
        }
    }
    else
    {
        for(i=0;i<strlen(ComBaud_Str[Com_Param.baud_index]);++i)
        {
            Disp_Chn816_Char(18+i,0,ComBaud_Str[Com_Param.baud_index][i]&0x0f,1);
        }
    }
	
	if(SndDis_Index < 1)
        return;
    
    //变量信息
    if(Input_Type == 0)
    {
        Disp_Chn1616_Str(9,30,index_id,1);
		Disp_Chn1616_Str(9,50,index_code,1);
        Disp_Chn1616_Str(9,70,index_addr,1);  
    }
    else
    {
        memset(pon,1,sizeof(pon));
        pon[ThdDis_Index] = 0;
        Disp_Chn1616_Str(9,30,index_id,pon[0]);
		Disp_Chn1616_Str(9,50,index_code,pon[1]);
        Disp_Chn1616_Str(9,70,index_addr,pon[2]);
    }
	
	//变量编码
    if((Input_Type == 2) && (ThdDis_Index == 0))
    {
        for(i=0;i<3;++i)   
        {
            char t;
            if(i == Input_Pos)
                t = 0;
            else 
                t = 1;
            
            if(Input_Temp[i] == 'E')
                Disp_Chn816_Char(19+i,30,28,t); 
            else if(Input_Temp[i] == 'C')
                Disp_Chn816_Char(19+i,30,38,t);
            else if(Input_Temp[i] == 'S')
                Disp_Chn816_Char(19+i,30,16,t);
			else if(Input_Temp[i] == 'P')
                Disp_Chn816_Char(19+i,30,17,t);
			else if(Input_Temp[i] == 'U')
                Disp_Chn816_Char(19+i,30,30,t);
			else if(Input_Temp[i] == 'I')
                Disp_Chn816_Char(19+i,30,19,t);
			else if(Input_Temp[i] == 'A')
                Disp_Chn816_Char(19+i,30,21,t);
			else if(Input_Temp[i] == 'B')
                Disp_Chn816_Char(19+i,30,26,t);
            else
                Disp_Chn816_Char(19+i,30,Input_Temp[i]&0x0f,t);
        }
    }
    else
    {
        for(i=0;i<3;++i)   
        {
            if(Com_Param.regid[SndDis_Index-1][i] == 'E')
                Disp_Chn816_Char(19+i,30,28,1); 
            else if(Com_Param.regid[SndDis_Index-1][i] == 'C')
                Disp_Chn816_Char(19+i,30,38,1);
            else if(Com_Param.regid[SndDis_Index-1][i] == 'S')
                Disp_Chn816_Char(19+i,30,16,1);
			else if(Com_Param.regid[SndDis_Index-1][i] == 'P')
                Disp_Chn816_Char(19+i,30,17,1);
			else if(Com_Param.regid[SndDis_Index-1][i] == 'U')
                Disp_Chn816_Char(19+i,30,30,1);
			else if(Com_Param.regid[SndDis_Index-1][i] == 'I')
                Disp_Chn816_Char(19+i,30,19,1);
			else if(Com_Param.regid[SndDis_Index-1][i] == 'A')
                Disp_Chn816_Char(19+i,30,21,1);
			else if(Com_Param.regid[SndDis_Index-1][i] == 'B')
                Disp_Chn816_Char(19+i,30,26,1);
            else
                Disp_Chn816_Char(19+i,30,Com_Param.regid[SndDis_Index-1][i]&0x0f,1);
        }   
    }
	
	//设备编号
	if((Input_Type == 2) && (ThdDis_Index == 1))
    {
        for(i=0;i<=Input_Pos;++i)   
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(19+i,50,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(19+i,50,Input_Temp[i]&0x0f,1);
        }
    }
    else
    {
        INT32U temp = Com_Param.devid[SndDis_Index-1];
        sprintf(str,"%d",temp);
        for(i=0;i<strlen(str);++i)
            Disp_Chn816_Char(19+i,50,str[i]&0x0f,1); 
    }
    
    //变量地址
    if((Input_Type == 2) && (ThdDis_Index == 2))
    {
        for(i=0;i<=Input_Pos;++i)   
        {
            if(i == Input_Pos)
                Disp_Chn816_Char(19+i,70,Input_Temp[i]&0x0f,0);
            else
                Disp_Chn816_Char(19+i,70,Input_Temp[i]&0x0f,1);
        }
    }
    else
    {
        INT32U temp = Com_Param.regaddr[SndDis_Index-1];
        sprintf(str,"%ld",temp);
        for(i=0;i<strlen(str);++i)
            Disp_Chn816_Char(19+i,70,str[i]&0x0f,1); 
    }
}

//Logo画面
void Dis_Logo(void)                
{
    Disp_Img(LCD_IMG_ADDR+960,30,64,zimo240128);
    Delay_N_mS(60000);
}

//总的显示处理程序
void LCD_Display(INT8U AllRefresh)
{
    switch(MainDis_Index)
    {
    case WELCOME_INDEX:
        Dis_Welcome(AllRefresh);  
        break;    
    case MAINSEL_INDEX:
        Dis_MainSel(AllRefresh);  
        break;   
    case STATWATCH_INDEX:
        Dis_StatWatch(AllRefresh);  
        break;   
    case DATAWATCH_INDEX:
        Dis_DataWatch(AllRefresh);  
        break;   
    case LOGWATCH_INDEX:
        Dis_LogWatch(AllRefresh);  
        break;   
    case PWINPUT_INDEX:
        Dis_PWInput(AllRefresh);  
        break;
	case CONTROL_INDEX:
        Dis_Control(AllRefresh);  
        break; 
    case SETUP_INDEX:
        Dis_Setup(AllRefresh);  
        break;   
    case AISETUP_INDEX:
        Dis_AISetup(AllRefresh);  
        break;   
    case WORKSETUP_INDEX:
        Dis_WorkSetup(AllRefresh);  
        break; 
    case DEVSETUP_INDEX:
        Dis_DevSetup(AllRefresh);  
        break; 
    case SYSSETUP_INDEX:
        Dis_SysSetup(AllRefresh);  
        break;   
    case COMSETUP_INDEX:
        Dis_ComSetup(AllRefresh);  
        break;     
    default:
        break;
    }
}

//按键处理部分
void Key_Welcome(INT8U KeyVal)
{
    if((KeyVal == KEY_ENTER) || (KeyVal == KEY_CANCEL))
    {
        MainDis_Index =  MAINSEL_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
}

//菜单选择
void Key_MainSel(INT8U KeyVal)
{
    if(KeyVal == KEY_ENTER)
    {
        //进入相应页面
        if(SndDis_Index == 0)
        {
            MainDis_Index = STATWATCH_INDEX; 
        }
        else if(SndDis_Index == 1)
        {
            MainDis_Index =  PWINPUT_INDEX;
            PWLast_Index = SETUP_INDEX;
            //MainDis_Index = SETUP_INDEX;
        }
		else if(SndDis_Index == 2)
        {
            MainDis_Index =  PWINPUT_INDEX;
            PWLast_Index = CONTROL_INDEX;
            //MainDis_Index = CONTROL_INDEX;
        }
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_CANCEL)
    {
        //返回主页面
        MainDis_Index =  WELCOME_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_UP)
    {
        //切换选择项
        if(SndDis_Index == 0)
            SndDis_Index = 2;
        else
            SndDis_Index--;
        Dis_PartRefresh = 1;  
    }
    else if(KeyVal == KEY_DOWN)
    {
        if(SndDis_Index == 2)
            SndDis_Index = 0;
        else
            SndDis_Index++;
        Dis_PartRefresh = 1;  
    }
    
    //后门程序，同时按住左、右、ESC可不用输入密码
    if((KeyVal != BACK_DOOR0) && (KeyVal != BACK_DOOR1))
        DisplayTimer = 0;
    if((KeyVal == BACK_DOOR1) && (DisplayTimer > 200))
    {
        if(SndDis_Index == 1)
            MainDis_Index = SETUP_INDEX;  
		else if(SndDis_Index == 2)
            MainDis_Index = CONTROL_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;   
    }
}

//状态显示
void Key_StatWatch(INT8U KeyVal)
{
    if(KeyVal == KEY_CANCEL)
    {
        MainDis_Index =  MAINSEL_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_UP)
    {
        MainDis_Index =  LOGWATCH_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_DOWN)
    {
        MainDis_Index =  DATAWATCH_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
}

//数据显示
void Key_DataWatch(INT8U KeyVal)
{
    if(KeyVal == KEY_CANCEL)
    {
        MainDis_Index =  MAINSEL_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_UP)
    {
        MainDis_Index =  STATWATCH_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_DOWN)
    {
        MainDis_Index =  LOGWATCH_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
}

//LOG查看
void Key_LogWatch(INT8U KeyVal)
{
    if(KeyVal == KEY_CANCEL)
    {
        MainDis_Index =  MAINSEL_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_UP)
    {
        MainDis_Index =  DATAWATCH_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_DOWN)
    {
        MainDis_Index =  STATWATCH_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
}

//密码输入
void Key_PWInput(INT8U KeyVal)
{
    if(KeyVal == KEY_CANCEL)
    {
        MainDis_Index =  MAINSEL_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_ENTER)
    {
        //判断密码是否正确
        if((Input_Pos == 5) &&
            (memcmp(Input_Temp, Sys_Param.pw, 6) == 0))
        {
            MainDis_Index =  PWLast_Index;  
        }
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_RIGHT)
    {
        //切换输入位置
        if(Input_Pos < 5)
        {
           Input_Pos++; 
           Dis_PartRefresh = 1;  
        }
    }
    else if(KeyVal == KEY_LEFT)
    {
        if(Input_Pos > 0)
        {
           Input_Pos--; 
           Dis_PartRefresh = 1;  
        }
    }
    else if(KeyVal == KEY_UP)
    {
        //切换输入字符
        if(Input_Temp[Input_Pos] > '0')
            Input_Temp[Input_Pos]--; 
        else
            Input_Temp[Input_Pos] = '9';
        Dis_PartRefresh = 1;
    }
    else if(KeyVal == KEY_DOWN)
    {
        if(Input_Temp[Input_Pos] < '9')
            Input_Temp[Input_Pos]++; 
        else
            Input_Temp[Input_Pos] = '0';
        Dis_PartRefresh = 1;
    }
}

//设备控制
void Key_Control(INT8U KeyVal)
{
    if(KeyVal == KEY_CANCEL)
    {
        MainDis_Index =  MAINSEL_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_ENTER)
    {
        Dis_PartRefresh = 1;
        
        //手动状态才允许操作
        if(Get_WorkType() != WORKTYPE_AUTO)
        {
            if(SndDis_Index < 2)
            {
                if(Get_PumpControlStat(SndDis_Index) == DEVICE_STOP)
                {
                    Start_Pump(SndDis_Index,0);
                }
                else if(Get_PumpControlStat(SndDis_Index) == DEVICE_RUN)
                {
                    Stop_Pump(SndDis_Index);
                }   
			}
            else if(SndDis_Index < 4)
            {
              	if(Get_EngineControlStat(SndDis_Index-2) == DEVICE_STOP)
                {
                    Start_Engine(SndDis_Index-2,0);
                }
                else if(Get_EngineControlStat(SndDis_Index-2) == DEVICE_RUN)
                {
                    Stop_Engine(SndDis_Index-2);
                }  
            }
            else if(SndDis_Index < 6)
            {    
                if(Get_ValueControlStat(SndDis_Index-4) == DEVICE_STOP)
                {
                    Start_Value(SndDis_Index-4,0);
                }
                else if(Get_ValueControlStat(SndDis_Index-4) == DEVICE_RUN)
                {
                    Stop_Value(SndDis_Index-4);
                }   
            }
            else if(SndDis_Index < 8)
            {
                if(Get_BackflowControlStat(SndDis_Index-6) == DEVICE_STOP)
                {
                    Start_Backflow(SndDis_Index-6,0);
                }
                else if(Get_BackflowControlStat(SndDis_Index-6) == DEVICE_RUN)
                {
                    Stop_Backflow(SndDis_Index-6);
                }  
            }
			else if(SndDis_Index < 10)
            {
                if(Get_ReuseControlStat(SndDis_Index-8) == DEVICE_STOP)
                {
                    Start_Reuse(SndDis_Index-8,0);
                }
                else if(Get_ReuseControlStat(SndDis_Index-8) == DEVICE_RUN)
                {
                    Stop_Reuse(SndDis_Index-8);
                }  
            }
			            
            Dis_PartRefresh = 1; 
        } 
    }
    else if((KeyVal == KEY_RIGHT) || (KeyVal == KEY_LEFT))
    {
        //切换操作选择项
        if(Get_WorkType() != WORKTYPE_AUTO)
        {
            if(SndDis_Index%2 == 0)
                SndDis_Index++;
            else
                SndDis_Index--;
            Dis_PartRefresh = 1; 
        }
    }
    else if(KeyVal == KEY_UP)
    {
        if(Get_WorkType() != WORKTYPE_AUTO)
        {
            if(SndDis_Index >= 2)
                SndDis_Index -= 2;
            Dis_PartRefresh = 1; 
        }
        
    }
    else if(KeyVal == KEY_DOWN)
    {
        if(Get_WorkType() != WORKTYPE_AUTO)
        {
            if(SndDis_Index <= 7)
                SndDis_Index += 2;
            Dis_PartRefresh = 1; 
        }
    }
}

//设置选择
void Key_Setup(INT8U KeyVal)
{
    if(KeyVal == KEY_ENTER)
    {
        if(SndDis_Index == 0)
            MainDis_Index = WORKSETUP_INDEX; 
        else if(SndDis_Index == 1)
            MainDis_Index = DEVSETUP_INDEX;
        else if(SndDis_Index == 2)
            MainDis_Index = AISETUP_INDEX;
        else if(SndDis_Index == 3)
            MainDis_Index = SYSSETUP_INDEX;
        else if(SndDis_Index == 4)
            MainDis_Index = COMSETUP_INDEX;
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_CANCEL)
    {
        MainDis_Index =  MAINSEL_INDEX;  
        Dis_AllRefresh = 1;  
        DisplayTimer = 0;
    }
    else if(KeyVal == KEY_RIGHT)
    {
        if(SndDis_Index < 4)
            SndDis_Index++;
        Dis_PartRefresh = 1; 
    }
    else if(KeyVal == KEY_LEFT)
    {
        if(SndDis_Index > 0)
            SndDis_Index--;
        Dis_PartRefresh = 1; 
    }
    else if(KeyVal == KEY_UP)
    {
        if(SndDis_Index == 3)
            SndDis_Index = 0;
        else if(SndDis_Index == 4)
            SndDis_Index = 2;
        Dis_PartRefresh = 1; 
    }
    else if(KeyVal == KEY_DOWN)
    {
        if(SndDis_Index < 2)
            SndDis_Index = 3;
        else if(SndDis_Index < 3)
            SndDis_Index = 4;
        Dis_PartRefresh = 1; 
    }
}

//AI设置
void Key_AISetup(INT8U KeyVal)
{
    if(KeyVal == KEY_ENTER)
    {
        if(Input_Type == 0)
        {
            Input_Type = 1;
            ThdDis_Index = 0;
            Dis_PartRefresh = 1; 
        }
        else if(Input_Type == 1)
        {
            //将当前参数值拷贝至临时设置变量
            Input_Type = 2; 
            Input_Pos = 0;
            Dis_PartRefresh = 1; 
            if(ThdDis_Index == 0)
            {
                Input_Temp[0] = Ad_Param[SndDis_Index].type;   
            }
            else if(ThdDis_Index == 1)
            {
                memcpy(Input_Temp,Ad_Param[SndDis_Index].id,3);  
            }
            else if(ThdDis_Index == 2)
            {
                memset(Input_Temp,'0',sizeof(Input_Temp));
            }
            else if(ThdDis_Index == 3)
            {
                memset(Input_Temp,'0',sizeof(Input_Temp));
            }
        }
        else if(Input_Type == 2)
        {
            //保存设置参数
            Input_Type = 1; 
            Dis_PartRefresh = 1;  
            
            if(ThdDis_Index == 0)
            {
                Ad_Param[SndDis_Index].type = Input_Temp[0];   
            }
            else if(ThdDis_Index == 1)
            {
                memcpy(Ad_Param[SndDis_Index].id,Input_Temp,3);
                Ad_Param[SndDis_Index].id[3] = 0;
            }
            else if(ThdDis_Index == 2)
            {
                Input_Temp[Input_Pos+1] = 0;
                Ad_Param[SndDis_Index].highval = atof(Input_Temp);
            }
            else if(ThdDis_Index == 3)
            {
                Input_Temp[Input_Pos+1] = 0;
                Ad_Param[SndDis_Index].lowval = atof(Input_Temp);
            }
            
            Save_ADParam(Ad_Param);
        }
    }
    else if(KeyVal == KEY_CANCEL)
    {
        if(Input_Type == 0)
        {
            MainDis_Index = SETUP_INDEX;  
            Dis_AllRefresh = 1;  
            DisplayTimer = 0;
        }
        else if(Input_Type == 1)
        {
            Input_Type = 0; 
            Dis_PartRefresh = 1; 
        }
        else if(Input_Type == 2)
        {
            Input_Type = 1; 
            Dis_PartRefresh = 1; 
        }
    }
    else if(KeyVal == KEY_RIGHT)
    {
        if(Input_Type == 2)
        {
            Dis_PartRefresh = 1; 
            
            if(ThdDis_Index == 1)
            {
                if(Input_Pos < 2)
                    Input_Pos++;
            }
            else if((ThdDis_Index == 2) || (ThdDis_Index == 3))
            {
                if(Input_Pos < 9)
                    Input_Pos++;
            }
        }
    }
    else if(KeyVal == KEY_LEFT)
    {
        //切换输入位置
        if(Input_Type == 2)
        {
            Dis_PartRefresh = 1; 
            
            if((ThdDis_Index == 1) || (ThdDis_Index == 2) ||
                (ThdDis_Index == 3))
            {
                if(Input_Pos > 0)
                    Input_Pos--;
            }
        } 
    }
    else if(KeyVal == KEY_UP)
    {
        if(Input_Type == 0)
        {
            if(SndDis_Index > 0)
            {
                SndDis_Index--;
                Dis_PartRefresh = 1; 
            }
        } 
        else if(Input_Type == 1)
        {
            if(ThdDis_Index > 0)
            {
                ThdDis_Index--;
                Dis_PartRefresh = 1; 
            }
        }
        else if(Input_Type == 2)
        {
            //切换输入数据
            Dis_PartRefresh = 1; 
            
            if(ThdDis_Index == 0)
            {
                if(Input_Temp[0] <= 0)
                    Input_Temp[0] = 2;
                else
                    Input_Temp[0]--;
            }
            else if(ThdDis_Index == 1)
            {
                if((Input_Temp[Input_Pos] > '0') && (Input_Temp[Input_Pos] <= '9'))
					Input_Temp[Input_Pos]--;
                else if(Input_Temp[Input_Pos] == '0')
                    Input_Temp[Input_Pos] = 'O';
				else if(Input_Temp[Input_Pos] == 'O')
                    Input_Temp[Input_Pos] = 'W';
				else if(Input_Temp[Input_Pos] == 'W')
                    Input_Temp[Input_Pos] = 'U';
                else if(Input_Temp[Input_Pos] == 'U')
                    Input_Temp[Input_Pos] = 'F';
                else if(Input_Temp[Input_Pos] == 'F')
                    Input_Temp[Input_Pos] = '9';
                else
                   Input_Temp[Input_Pos] = '0';  
            }
            else if((ThdDis_Index == 2) || (ThdDis_Index == 3))
            {
                if(Input_Temp[Input_Pos] == '0')
                   Input_Temp[Input_Pos] = '.';
                else if(Input_Temp[Input_Pos] == '.')
                   Input_Temp[Input_Pos] = '9';
                else
                   Input_Temp[Input_Pos]--; 
            }   
        }
    }
    else if(KeyVal == KEY_DOWN)
    {
        if(Input_Type == 0)
        {
            if(SndDis_Index < 7)
            {
                SndDis_Index++;
                Dis_PartRefresh = 1; 
            }
        } 
        else if(Input_Type == 1)
        {
            if(ThdDis_Index < 3)
            {
                ThdDis_Index++;
                Dis_PartRefresh = 1; 
            }
        }
        else if(Input_Type == 2)
        {
            Dis_PartRefresh = 1; 
            
            if(ThdDis_Index == 0)
            {
                if(Input_Temp[0] >= 2)
                    Input_Temp[0] = 0;
                else
                    Input_Temp[0]++;
            }
            else if(ThdDis_Index == 1)
            {
                if((Input_Temp[Input_Pos] >= '0') && (Input_Temp[Input_Pos] < '9'))
					Input_Temp[Input_Pos]++; 
                else if(Input_Temp[Input_Pos] == '9')
                    Input_Temp[Input_Pos] = 'F';
                else if(Input_Temp[Input_Pos] == 'F')
                    Input_Temp[Input_Pos] = 'U';
                else if(Input_Temp[Input_Pos] == 'U')
                    Input_Temp[Input_Pos] = 'W';
				else if(Input_Temp[Input_Pos] == 'W')
                    Input_Temp[Input_Pos] = 'O';
				else if(Input_Temp[Input_Pos] == 'O')
                    Input_Temp[Input_Pos] = '0';
                else
                    Input_Temp[Input_Pos] = '0';
            }
            else if((ThdDis_Index == 2) || (ThdDis_Index == 3))
            {
                if(Input_Temp[Input_Pos] == '9')
                   Input_Temp[Input_Pos] = '.';
                else if(Input_Temp[Input_Pos] == '.')
                   Input_Temp[Input_Pos] = '0';
                else
                   Input_Temp[Input_Pos]++; 
            }
        }
    }
 
}

//工作参数
void Key_WorkSetup(INT8U KeyVal)
{
    if(KeyVal == KEY_ENTER)
    {
        if(Input_Type == 0)
        {
            //将当前参数值拷贝至临时输入变量
            Input_Type = 1; 
            Input_Pos = 0;
            Dis_PartRefresh = 1;
            
			if(SndDis_Index == 0)
            {
                Input_Temp[0] = Sys_Param.worktype;
            }
            else if(SndDis_Index <= 5)
            {
                memset(Input_Temp,'0',sizeof(Input_Temp));  
            }
        }
        else if(Input_Type == 1)
        {
            //保存输入参数
            Input_Type = 0; 
            Dis_PartRefresh = 1;  
            
			if(SndDis_Index == 0)
            {
                Sys_Param.worktype = Input_Temp[0];
            }
            else if(SndDis_Index == 1)
            {
                Input_Temp[Input_Pos+1] = 0;
                Sys_Param.dohigh = atof(Input_Temp);   
            }
			else if(SndDis_Index == 2)
            {
                Input_Temp[Input_Pos+1] = 0;
                Sys_Param.dolow = atof(Input_Temp);   
            }
            else if(SndDis_Index == 3)
            {
                Input_Temp[Input_Pos+1] = 0;
                Sys_Param.engine_switchlong = atoi(Input_Temp);
            }
            else if(SndDis_Index == 4)
            {
                Input_Temp[Input_Pos+1] = 0;
                Sys_Param.value_switchlong = atoi(Input_Temp);
            }
            else if(SndDis_Index == 5)
            {
                Input_Temp[Input_Pos+1] = 0;
                Sys_Param.value_switchlast = atoi(Input_Temp);
            }
                        
            Save_SysParam(&Sys_Param);
        }
    }
    else if(KeyVal == KEY_CANCEL)
    {
        if(Input_Type == 0)
        {
            MainDis_Index = SETUP_INDEX;  
            Dis_AllRefresh = 1;  
            DisplayTimer = 0;
        }
        else if(Input_Type == 1)
        {
            Input_Type = 0; 
            Dis_PartRefresh = 1; 
        }
    }
    else if(KeyVal == KEY_RIGHT)
    {
        if(Input_Type == 1)
        {
            Dis_PartRefresh = 1; 
			
			if(SndDis_Index == 0)
			{
			}
			else if(SndDis_Index <= 2)
            {
                if(Input_Pos < 9)
                    Input_Pos++;
            }
            else if(SndDis_Index <= 5)
            {
                if(Input_Pos < 2)
                    Input_Pos++;
            }
		}
    }
    else if(KeyVal == KEY_LEFT)
    {
        //切换输入位置
        if(Input_Type == 1)
        {
            Dis_PartRefresh = 1; 
            
			if(SndDis_Index >= 1)
			{
            	if(Input_Pos > 0)
            		Input_Pos--;
			}
        } 
    }
    else if(KeyVal == KEY_UP)
    {
        if(Input_Type == 0)
        {
            if(SndDis_Index > 0)
            {
                SndDis_Index--;
                Dis_PartRefresh = 1; 
            }
        } 
        else if(Input_Type == 1)
        {
            //切换输入数据
            Dis_PartRefresh = 1; 
            
            if(SndDis_Index == 0)
			{
				if(Input_Temp[0] == WORKTYPE_AUTO)
                    Input_Temp[0] = WORKTYPE_REMOTE;
                else
                    Input_Temp[0] = WORKTYPE_AUTO;
			}
			else if(SndDis_Index <= 2)
            {
                if(Input_Temp[Input_Pos] == '0')
                   Input_Temp[Input_Pos] = '.';
                else if(Input_Temp[Input_Pos] == '.')
                   Input_Temp[Input_Pos] = '9';
                else
                   Input_Temp[Input_Pos]--; 
            }
            else if(SndDis_Index <= 5)
            {
                if((Input_Temp[Input_Pos] < '0') || (Input_Temp[Input_Pos] > '9'))
                    Input_Temp[Input_Pos] = '0';
                else if(Input_Temp[Input_Pos] == '0')
                    Input_Temp[Input_Pos] = '9';
                else
                   Input_Temp[Input_Pos]--;
            }
        }
    }
    else if(KeyVal == KEY_DOWN)
    {
        if(Input_Type == 0)
        {
            if(SndDis_Index < 5)
            {
                SndDis_Index++;
                Dis_PartRefresh = 1; 
            }
        } 
        else if(Input_Type == 1)
        {
            Dis_PartRefresh = 1; 
            
			if(SndDis_Index == 0)
			{
				if(Input_Temp[0] == WORKTYPE_AUTO)
                    Input_Temp[0] = WORKTYPE_REMOTE;
                else
                    Input_Temp[0] = WORKTYPE_AUTO;
			}
            else if(SndDis_Index <= 2)
            {
                if(Input_Temp[Input_Pos] == '9')
                   Input_Temp[Input_Pos] = '.';
                else if(Input_Temp[Input_Pos] == '.')
                   Input_Temp[Input_Pos] = '0';
                else
                   Input_Temp[Input_Pos]++; 
            }
            else if(SndDis_Index <= 5)
            {
                if((Input_Temp[Input_Pos] < '0') || (Input_Temp[Input_Pos] > '9'))
                    Input_Temp[Input_Pos] = '0';
                else if(Input_Temp[Input_Pos] == '9')
                    Input_Temp[Input_Pos] = '0';
                else
                   Input_Temp[Input_Pos]++; 
                
            }
        }
    }
 
}

//设备设置
void Key_DevSetup(INT8U KeyVal)
{
    if(KeyVal == KEY_ENTER)
    {
        if(Input_Type == 0)
        {
            //将当前设置拷贝至临时输入变量
            Input_Type = 1; 
            Input_Pos = 0;
            Dis_PartRefresh = 1;
            
            if(SndDis_Index < 2)
            {
                Input_Temp[0] = (Sys_Param.pumpen>>SndDis_Index) & 0x01;
            }
            else if(SndDis_Index < 4)
            {
                Input_Temp[0] = (Sys_Param.engineen>>(SndDis_Index-2)) & 0x01;
            }
            else if(SndDis_Index < 6)
            {
                Input_Temp[0] = (Sys_Param.valueen>>(SndDis_Index-4)) & 0x01;
            }
			else if(SndDis_Index < 8)
            {
                Input_Temp[0] = (Sys_Param.backflowen>>(SndDis_Index-6)) & 0x01;
            }
			else if(SndDis_Index < 10)
            {
                Input_Temp[0] = (Sys_Param.reuseen>>(SndDis_Index-8)) & 0x01;
            }
			else if(SndDis_Index < 14)
            {
                Input_Temp[0] = (Sys_Param.levelen>>(SndDis_Index-10)) & 0x01;
            }
		}
        else if(Input_Type == 1)
        {
            Input_Type = 0; 
            Dis_PartRefresh = 1;
            
            //保存设置参数
            if(SndDis_Index < 2)
            {
                if(Input_Temp[0] == 0)
                    Sys_Param.pumpen &= (~(1<<SndDis_Index));
                else
                    Sys_Param.pumpen |= (1<<SndDis_Index);
            }
            else if(SndDis_Index < 4)
            {
                if(Input_Temp[0] == 0)
                    Sys_Param.engineen &= (~(1<<(SndDis_Index-2)));
                else
                    Sys_Param.engineen |= (1<<(SndDis_Index-2));
            }
            else if(SndDis_Index < 6)
            {
                if(Input_Temp[0] == 0)
                    Sys_Param.valueen &= (~(1<<(SndDis_Index-4)));
                else
                    Sys_Param.valueen |= (1<<(SndDis_Index-4));
            }
			else if(SndDis_Index < 8)
            {
                if(Input_Temp[0] == 0)
                    Sys_Param.backflowen &= (~(1<<(SndDis_Index-6)));
                else
                    Sys_Param.backflowen |= (1<<(SndDis_Index-6));
            }
			else if(SndDis_Index < 10)
            {
                if(Input_Temp[0] == 0)
                    Sys_Param.reuseen &= (~(1<<(SndDis_Index-8)));
                else
                    Sys_Param.reuseen |= (1<<(SndDis_Index-8));
            }
			else if(SndDis_Index < 14)
            {
                if(Input_Temp[0] == 0)
                    Sys_Param.levelen &= (~(1<<(SndDis_Index-10)));
                else
                    Sys_Param.levelen |= (1<<(SndDis_Index-10));
            }
			            
            Save_SysParam(&Sys_Param);
        }
    }
    else if(KeyVal == KEY_CANCEL)
    {
        if(Input_Type == 0)
        {
            MainDis_Index = SETUP_INDEX;  
            Dis_AllRefresh = 1;  
            DisplayTimer = 0;
        }
        else if(Input_Type == 1)
        {
            Input_Type = 0; 
            Dis_PartRefresh = 1; 
        }
    }
    else if(KeyVal == KEY_UP)
    {
        if(Input_Type == 0)
        {
            if(SndDis_Index > 0)
            {
                SndDis_Index--;
                Dis_PartRefresh = 1; 
            }
        } 
        else if(Input_Type == 1)
        {
            //切换输入数据
            Dis_PartRefresh = 1; 
            Input_Temp[0] = (!Input_Temp[0]);
        }
    }
    else if(KeyVal == KEY_DOWN)
    {
        if(Input_Type == 0)
        {
            if(SndDis_Index < 13)
            {
                SndDis_Index++;
                Dis_PartRefresh = 1; 
            }
        } 
        else if(Input_Type == 1)
        {
            Dis_PartRefresh = 1; 
            Input_Temp[0] = (!Input_Temp[0]);
        }
    }
	else if((KeyVal == KEY_LEFT) || (KeyVal == KEY_RIGHT))
    {
        if(Input_Type == 0)
        {
            if(SndDis_Index > 7)
			{
                SndDis_Index -= 8;
			}
            else
			{	
				if((SndDis_Index+8) < 14)
					SndDis_Index += 8;
			}
			Dis_PartRefresh = 1;
        } 
    }
}

//系统设置
void Key_SysSetup(INT8U KeyVal)
{
    if(KeyVal == KEY_ENTER)
    {
        if(Input_Type == 0)
        {
            //将当前设置拷贝至临时输入变量
            Input_Type = 1; 
            Input_Pos = 0;
            Dis_PartRefresh = 1; 
            if(SndDis_Index == 0)
            {
                memset(Input_Temp,'0',sizeof(Input_Temp));   
            }
            else if(SndDis_Index == 1)
            {
                memcpy(Input_Temp,System_TimeStr,14);  
            }
            else if(SndDis_Index == 2)
            {
                memcpy(Input_Temp,Sys_Param.sim,11);
            }
            else if(SndDis_Index == 3)
            {
                memcpy(Input_Temp,Sys_Param.mn,14);
            }
            else if(SndDis_Index == 4)
            {
                memcpy(Input_Temp,Sys_Param.st,2);
            }
            else if(SndDis_Index == 5)
            {
                memcpy(Input_Temp,Sys_Param.rtd,3);
            }
        }
        else if(Input_Type == 1)
        {
            //保存设置参数
            if(SndDis_Index == 0)
            {
                if(Input_Pos == 5)
                {    
                    Input_Temp[6] = 0;
                    strcpy(Sys_Param.pw,Input_Temp);
                    Input_Type = 0; 
                    Dis_PartRefresh = 1;
                    Save_SysParam(&Sys_Param);
                }
            }
            else if(SndDis_Index == 1)
            {
                Input_Temp[14] = 0;
                v_Set1302((INT8U *)Input_Temp);
                v_Get1302(System_TimeStr, &Current_Tm);
                Input_Type = 0; 
                Dis_PartRefresh = 1;
            }
            else if(SndDis_Index == 2)
            {
                Input_Temp[11] = 0;
                strcpy(Sys_Param.sim,Input_Temp);
                Input_Type = 0; 
                Dis_PartRefresh = 1;
                Save_SysParam(&Sys_Param);  
            }
            else if(SndDis_Index == 3)
            {
                Input_Temp[14] = 0;
                strcpy(Sys_Param.mn,Input_Temp);
                Input_Type = 0; 
                Dis_PartRefresh = 1;
                Save_SysParam(&Sys_Param);  
            }
            else if(SndDis_Index == 4)
            {
                Input_Temp[2] = 0;
                strcpy(Sys_Param.st,Input_Temp);
                Input_Type = 0; 
                Dis_PartRefresh = 1;
                Save_SysParam(&Sys_Param);
            }
            else if(SndDis_Index == 5)
            {
                Input_Temp[3] = 0;
                strcpy(Sys_Param.rtd,Input_Temp);
                Input_Type = 0; 
                Dis_PartRefresh = 1;
                Save_SysParam(&Sys_Param);
            }
        }
    }
    else if(KeyVal == KEY_CANCEL)
    {
        if(Input_Type == 0)
        {
            MainDis_Index = SETUP_INDEX;  
            Dis_AllRefresh = 1;  
            DisplayTimer = 0;
        }
        else if(Input_Type == 1)
        {
            Input_Type = 0; 
            Dis_PartRefresh = 1; 
        }
    }
    else if(KeyVal == KEY_RIGHT)
    {
        //切换输入位置
        if(Input_Type == 1)
        {
            Dis_PartRefresh = 1; 
            
            if(SndDis_Index == 0)
            {
                if(Input_Pos < 5)
                    Input_Pos++;
            }
            else if((SndDis_Index == 1) || (SndDis_Index == 3))
            {
                if(Input_Pos < 13)
                    Input_Pos++;
            }
            else if(SndDis_Index == 2)
            {
                if(Input_Pos < 10)
                    Input_Pos++;
            }
            else if(SndDis_Index == 4)
            {
                if(Input_Pos < 1)
                    Input_Pos++;
            }
            else if(SndDis_Index == 5)
            {
                if(Input_Pos < 2)
                    Input_Pos++;
            }
        }
    }
    else if(KeyVal == KEY_LEFT)
    {
        if(Input_Type == 1)
        {
            Dis_PartRefresh = 1; 
            
            if(SndDis_Index <= 5)
            {
                if(Input_Pos > 0)
                    Input_Pos--;
            }
        } 
    }
    else if(KeyVal == KEY_UP)
    {
        if(Input_Type == 0)
        {
            if(SndDis_Index > 0)
            {
                SndDis_Index--;
                Dis_PartRefresh = 1; 
            }
        } 
        else if(Input_Type == 1)
        {
            //切换输入数据
            Dis_PartRefresh = 1; 
            
            if(SndDis_Index <= 5)
            {
                if((Input_Temp[Input_Pos] < '0') || (Input_Temp[Input_Pos] > '9'))
                    Input_Temp[Input_Pos] = '0';
                else if(Input_Temp[Input_Pos] == '0')
                    Input_Temp[Input_Pos] = '9';
                else
                   Input_Temp[Input_Pos]--;
            }
        }
    }
    else if(KeyVal == KEY_DOWN)
    {
        if(Input_Type == 0)
        {
            if(SndDis_Index < 5)
            {
                SndDis_Index++;
                Dis_PartRefresh = 1; 
            }
        } 
        else if(Input_Type == 1)
        {
            Dis_PartRefresh = 1; 
            
            if(SndDis_Index <= 5)
            {
                if((Input_Temp[Input_Pos] < '0') || (Input_Temp[Input_Pos] > '9'))
                    Input_Temp[Input_Pos] = '0';
                else if(Input_Temp[Input_Pos] == '9')
                    Input_Temp[Input_Pos] = '0';
                else
                   Input_Temp[Input_Pos]++; 
            }
        }
    }
}

//通信设置
void Key_ComSetup(INT8U KeyVal)
{
    if(KeyVal == KEY_ENTER)
    {
        if(Input_Type == 0)
        {
            //将当前设置拷贝至临时输入变量
            Input_Type = 1; 
            Input_Pos = 0;
            Dis_PartRefresh = 1; 
            if(SndDis_Index == 0)
            {
                memset(Input_Temp,'0',sizeof(Input_Temp)); 
                Input_Temp[0] = Com_Param.baud_index;
            }
            else if((SndDis_Index >= 1) && (SndDis_Index <= MODBUS_VARNUM))
            {
                ThdDis_Index = 0;
            }
        }
        else if(Input_Type == 1)
        {
            if(SndDis_Index == 0)
            {
                //保存设置参数
                Input_Type = 0; 
                Dis_PartRefresh = 1;
            
                Com_Param.baud_index = Input_Temp[0];
                Save_ComParam(&Com_Param);
            }
            else if(SndDis_Index <= MODBUS_VARNUM)
            {
                Input_Type = 2; 
                Input_Pos = 0;
                Dis_PartRefresh = 1; 
                
                if(ThdDis_Index == 0)
                    memcpy(Input_Temp,Com_Param.regid[SndDis_Index-1],3);      
                else if(ThdDis_Index <= 2)
                    memset(Input_Temp,'0',sizeof(Input_Temp));   
            }
        }
        else if(Input_Type == 2)
        {
            //保存设置参数
            Input_Type = 1; 
            Dis_PartRefresh = 1;  
            
            if(ThdDis_Index == 0)
            {
                Input_Temp[3] = 0; 
                strcpy(Com_Param.regid[SndDis_Index-1],Input_Temp);      
            }
            else if(ThdDis_Index == 1)
            {
                Input_Temp[Input_Pos+1] = 0;   
                Com_Param.devid[SndDis_Index-1] = atoi(Input_Temp);
            }
			else if(ThdDis_Index == 2)
            {
                Input_Temp[Input_Pos+1] = 0;   
                Com_Param.regaddr[SndDis_Index-1] = atol(Input_Temp);
            }
            Save_ComParam(&Com_Param);
        }
    }
    else if(KeyVal == KEY_CANCEL)
    {
        if(Input_Type == 0)
        {
            MainDis_Index = SETUP_INDEX;  
            Dis_AllRefresh = 1;  
            DisplayTimer = 0;
        }
        else if(Input_Type == 1)
        {
            Input_Type = 0; 
            Dis_PartRefresh = 1; 
        }
        else if(Input_Type == 2)
        {
            Input_Type = 1; 
            Dis_PartRefresh = 1; 
        }
    }
    else if(KeyVal == KEY_RIGHT)
    {
        if(Input_Type == 2)
        {
            Dis_PartRefresh = 1;
            if(ThdDis_Index <= 1)   
            {
                if(Input_Pos < 2)
                    Input_Pos++;   
            }
            else if(ThdDis_Index == 2)   
            {
                if(Input_Pos < 4)
                    Input_Pos++;   
            }
        }
    }
    else if(KeyVal == KEY_LEFT)
    {
        if(Input_Type == 2)
        {
            Dis_PartRefresh = 1;
            if(ThdDis_Index <= 2)   
            {
                if(Input_Pos > 0)
                    Input_Pos--;
            }
        }
    }
    else if(KeyVal == KEY_UP)
    {
        if(Input_Type == 0)
        {
            if(SndDis_Index > 0)
            {
                SndDis_Index--;
                Dis_PartRefresh = 1; 
            }
        } 
        else if(Input_Type == 1)
        {
            Dis_PartRefresh = 1; 
            
            if(SndDis_Index == 0)
            {
                //切换输入数据
                if(Input_Temp[0] > 0)          
                    Input_Temp[0]--; 
                else
                    Input_Temp[0] = 3;
            }
            else if((SndDis_Index >= 1) && (SndDis_Index <= MODBUS_VARNUM))
            {
                if(ThdDis_Index > 0)
                    ThdDis_Index--;
            }
        }
        else if(Input_Type == 2)
        {
            Dis_PartRefresh = 1;
            
            if(ThdDis_Index == 0)
            {
                if((Input_Temp[Input_Pos] > '0') && (Input_Temp[Input_Pos] <= '9'))
                    Input_Temp[Input_Pos]--;
				else if(Input_Temp[Input_Pos] == '0')
                    Input_Temp[Input_Pos] = 'B';
				else if(Input_Temp[Input_Pos] == 'B')
                    Input_Temp[Input_Pos] = 'A';
				else if(Input_Temp[Input_Pos] == 'A')
                    Input_Temp[Input_Pos] = 'I';
				else if(Input_Temp[Input_Pos] == 'I')
                    Input_Temp[Input_Pos] = 'U';
				else if(Input_Temp[Input_Pos] == 'U')
                    Input_Temp[Input_Pos] = 'P';
				else if(Input_Temp[Input_Pos] == 'P')
                    Input_Temp[Input_Pos] = 'M';
				else if(Input_Temp[Input_Pos] == 'M')
                    Input_Temp[Input_Pos] = 'T';
                else if(Input_Temp[Input_Pos] == 'T')
                    Input_Temp[Input_Pos] = 'S';
                else if(Input_Temp[Input_Pos] == 'S')
                    Input_Temp[Input_Pos] = 'C';
                else if(Input_Temp[Input_Pos] == 'C')
                    Input_Temp[Input_Pos] = 'E';
                else if(Input_Temp[Input_Pos] == 'E')
                    Input_Temp[Input_Pos] = '9';
                else
                    Input_Temp[Input_Pos] = '0';
            }  
            else if(ThdDis_Index <= 2)
            {
                if((Input_Temp[Input_Pos] < '0') || (Input_Temp[Input_Pos] > '9'))
                    Input_Temp[Input_Pos] = '0';
                else if(Input_Temp[Input_Pos] == '0')
                    Input_Temp[Input_Pos] = '9';
                else
                   Input_Temp[Input_Pos]--;
            }   
        }
    }
    else if(KeyVal == KEY_DOWN)
    {
        Dis_PartRefresh = 1; 
        
        if(Input_Type == 0)
        {
            if(SndDis_Index < MODBUS_VARNUM)
                SndDis_Index++;   
        } 
        else if(Input_Type == 1)
        {
            Dis_PartRefresh = 1; 
            
            if(SndDis_Index == 0)
            {
                if(Input_Temp[0] < 3)          
                    Input_Temp[0]++;    
                else
                    Input_Temp[0] = 0;
            }
            else if((SndDis_Index >= 1) && (SndDis_Index <= MODBUS_VARNUM))
            {
                if(ThdDis_Index < 2)
                    ThdDis_Index++;
            }
        }
        else if(Input_Type == 2)
        {
            Dis_PartRefresh = 1;
            
            if(ThdDis_Index == 0)
            {
                if((Input_Temp[Input_Pos] >= '0') && (Input_Temp[Input_Pos] < '9'))
                    Input_Temp[Input_Pos]++;
                else if(Input_Temp[Input_Pos] == '9')
                    Input_Temp[Input_Pos] = 'E';
                else if(Input_Temp[Input_Pos] == 'E')
                    Input_Temp[Input_Pos] = 'C';
                else if(Input_Temp[Input_Pos] == 'C')
                    Input_Temp[Input_Pos] = 'S';
                else if(Input_Temp[Input_Pos] == 'S')
                    Input_Temp[Input_Pos] = 'T';
				else if(Input_Temp[Input_Pos] == 'T')
                    Input_Temp[Input_Pos] = 'M';
				else if(Input_Temp[Input_Pos] == 'M')
                    Input_Temp[Input_Pos] = 'P';
				else if(Input_Temp[Input_Pos] == 'P')
                    Input_Temp[Input_Pos] = 'U';
				else if(Input_Temp[Input_Pos] == 'U')
                    Input_Temp[Input_Pos] = 'I';
				else if(Input_Temp[Input_Pos] == 'I')
                    Input_Temp[Input_Pos] = 'A';
				else if(Input_Temp[Input_Pos] == 'A')
                    Input_Temp[Input_Pos] = 'B';
				else if(Input_Temp[Input_Pos] == 'B')
                    Input_Temp[Input_Pos] = '0';
                else
                    Input_Temp[Input_Pos] = '0';
            }  
            else if(ThdDis_Index <= 2)
            {
                if((Input_Temp[Input_Pos] < '0') || (Input_Temp[Input_Pos] > '9'))
                    Input_Temp[Input_Pos] = '0';
                else if(Input_Temp[Input_Pos] == '9')
                    Input_Temp[Input_Pos] = '0';
                else
                   Input_Temp[Input_Pos]++; 
            }     
        }
    }
}

//总的键盘处理程序
void Key_Proc(INT8U KeyVal)
{
    switch(MainDis_Index)
    {
    case WELCOME_INDEX:
        Key_Welcome(KeyVal);  
        break;    
    case MAINSEL_INDEX:
        Key_MainSel(KeyVal);  
        break;   
    case STATWATCH_INDEX:
        Key_StatWatch(KeyVal);  
        break;   
    case DATAWATCH_INDEX:
        Key_DataWatch(KeyVal);  
        break;   
    case LOGWATCH_INDEX:
        Key_LogWatch(KeyVal);  
        break;   
    case PWINPUT_INDEX:
        Key_PWInput(KeyVal);  
        break; 
	case CONTROL_INDEX:
        Key_Control(KeyVal);  
        break; 
    case SETUP_INDEX:
        Key_Setup(KeyVal);  
        break;   
    case AISETUP_INDEX:
        Key_AISetup(KeyVal);  
        break;   
    case WORKSETUP_INDEX:
        Key_WorkSetup(KeyVal);  
        break; 
    case DEVSETUP_INDEX:
        Key_DevSetup(KeyVal);  
        break; 
    case SYSSETUP_INDEX:
        Key_SysSetup(KeyVal);  
        break;   
    case COMSETUP_INDEX:
        Key_ComSetup(KeyVal);  
        break;     
    default:
        break;
    }
}


//总处理函数
void ProcDisplay(void)
{
    INT8U keyval;
    
    keyval = Get_KeyVal();
    if(keyval == 0)
    {
        //120s无按键操作则关闭背光返回欢迎界面
        if(BackLightTimer >= S2TENMS(120))     
        {
            BackLightTimer = 0;
            LCD_LIGHT_OFF;
            if((MainDis_Index == CONTROL_INDEX) ||
                (MainDis_Index == SETUP_INDEX) ||
                (MainDis_Index == WORKSETUP_INDEX) ||
                (MainDis_Index == DEVSETUP_INDEX) ||    
                (MainDis_Index == AISETUP_INDEX) ||
                (MainDis_Index == SYSSETUP_INDEX) ||
                (MainDis_Index == COMSETUP_INDEX))
            {
                MainDis_Index =  WELCOME_INDEX;  
                Dis_AllRefresh = 1;  
                DisplayTimer = 0;   
            }
        }
        
        //950ms定时刷新界面
        if((MainDis_Index == WELCOME_INDEX) ||
            (MainDis_Index == STATWATCH_INDEX) ||
            (MainDis_Index == DATAWATCH_INDEX) ||
			(MainDis_Index == LOGWATCH_INDEX) ||
            (MainDis_Index == CONTROL_INDEX))
        {
            if(DisplayTimer >= MS2TENMS(950))
            {
                DisplayTimer = 0;
                Dis_PartRefresh = 1;      
            }
        }
        
        if((Dis_AllRefresh == 1) || (Dis_PartRefresh == 1))
        {
            LCD_Display(Dis_AllRefresh); 
            Dis_AllRefresh = 0;
            Dis_PartRefresh = 0;
        }
        
        return;
    }
       
    //有按键按下
    BackLightTimer = 0;
    LCD_LIGHT_ON;
    
    Key_Proc(keyval);       //处理按键事件
        
    if((Dis_AllRefresh == 1) || (Dis_PartRefresh == 1))
    {
        LCD_Display(Dis_AllRefresh); 
        Dis_AllRefresh = 0;
        Dis_PartRefresh = 0;
    }
}

