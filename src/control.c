

#include "system.h"
#include "control.h"
#include "dio.h"
#include "ad.h"
#include "at24c64.h"
#include "dio.h"

SYS_PARAM Sys_Param;

INT32U EngineRunTimer;

//��������ʱ��֮��ķ��Ӳ�ֵ
int CalcMinutePast(SWITCH_TIME *pNow, SWITCH_TIME *pBase)
{
	int min;
	
	if(pNow->starthour > pBase->starthour)
	{
		min = (pNow->starthour-pBase->starthour)*60 -
			pBase->startmin + pNow->startmin;
	}
	else if(pNow->starthour < pBase->starthour)
	{
		min = (24-pBase->starthour)*60 - pBase->startmin + 
			pNow->starthour*60 + pNow->startmin;	
	}
	else
	{
		if(pNow->startmin >= pBase->startmin)
		{
			min = (pNow->startmin-pBase->startmin);
		}
		else
		{
			min = 1440 - (pBase->startmin-pNow->startmin);
		}
	}
	
	return min;
}

void ClearAllControlTimer(void)
{
}

//��ʼ������״̬��Ϣ
void ControlInit(void)
{
}

//���������
void ProcessControlTime(void)
{
    static int LastSecond;
    int secnow; 

    secnow = Current_Tm.second;
    if(LastSecond == secnow)
		return;   
    
    LastSecond = secnow;
    
    //1s����������ֵ��1
	EngineRunTimer++;
}

//���Ƶ������
void ProcessControlErr(void)
{
    char i;
	
	if(Get_WorkType() != WORKTYPE_AUTO)
        return;
    
    for(i=0;i<2;++i)
    {
        if(Get_PumpStat(i) == DEVICE_FAULT)
            Stop_Pump(i);  
    }
    for(i=0;i<2;++i)
    {
        if(Get_EngineStat(i) == DEVICE_FAULT)
            Stop_Engine(i);  
    }
    for(i=0;i<2;++i)
    {
        if(Get_ValueStat(i) == DEVICE_FAULT)
            Stop_Value(i);  
    }
	for(i=0;i<2;++i)
    {
        if(Get_BackflowStat(i) == DEVICE_FAULT)
            Stop_Backflow(i);  
    }
    for(i=0;i<2;++i)
    {
        if(Get_ReuseStat(i) == DEVICE_FAULT)
            Stop_Reuse(i);  
    }
}

//�����ÿ���
void ProcessPump(void)
{
	if(Get_LevelStat(0) == LEVEL_HIGH)
	{
		Start_PumpAuto();
	}
	else if(Get_LevelStat(0) == LEVEL_LOW)
	{
		Stop_PumpAuto();
	}
}

//�����ÿ���
void ProcessBackflow(void)
{
	if(Get_LevelStat(1) == LEVEL_HIGH)
	{
		Start_BackflowAuto(0);
	}
	else if(Get_LevelStat(1) == LEVEL_LOW)
	{
		Stop_BackflowAuto(0);
	}
	
	if(Get_LevelStat(2) == LEVEL_HIGH)
	{
		Start_BackflowAuto(1);
	}
	else if(Get_LevelStat(2) == LEVEL_LOW)
	{
		Stop_BackflowAuto(1);
	}
}

//���ñÿ���
void ProcessReuse(void)
{
	if(Get_LevelStat(3) == LEVEL_HIGH)
	{
		Start_ReuseAuto();
	}
	else if(Get_LevelStat(3) == LEVEL_LOW)
	{
		Stop_ReuseAuto();
	}
}

void ProcessEngine(void)
{
	float val;
	int runflag;
	
	val = GetO2Val(0);
	if(val >= Sys_Param.dohigh)
	{
		runflag = 0;
	}
	else if(val <= Sys_Param.dolow)
	{
		runflag = 1;
	}
	else
	{
		if((Get_EngineStat(0)==DEVICE_RUN) || (Get_EngineStat(1)==DEVICE_RUN))
			runflag = 1;
		else 
			runflag = 0;
	}
	
	if(0 == runflag)
	{
		Stop_EngineAuto();
		EngineRunTimer = 0;
	}
	else
	{
		if(EngineRunTimer >= Sys_Param.engine_switchlong*60)
		{
			EngineRunTimer = 0;
			Start_EngineAuto(1);	//�л�������
		}
		else
		{
			Start_EngineAuto(0);
		}
	}
}


//��ŷ�����
static SWITCH_TIME Value_BaseTime = {0, 0};
void ProcessValue(void)
{
	SWITCH_TIME time_current;
	int timeval;
	
	time_current.starthour = Current_Tm.hour;
	time_current.startmin = Current_Tm.minute;
	
	timeval = CalcMinutePast(&time_current, &Value_BaseTime);
	if((timeval%Sys_Param.value_switchlong) < Sys_Param.value_switchlast)
	{
		Start_ValueAuto();
	}
	else
	{
		Stop_ValueAuto();
	}	
}

//�������̴���
void ProcessControlWater(void)
{
    if(Get_WorkType() != WORKTYPE_AUTO)
        return;
   
	//�������̿���
	ProcessPump();
	ProcessReuse();
	ProcessEngine();
	ProcessBackflow();
	ProcessValue();
}

void ProcControl(void)
{
    ProcessControlTime(); 
    ProcessControlErr();
    ProcessControlWater();
}

/////////////////////////////////////////////
//     ���ݿ������������ź�״̬

#define WORKTYPE_BIT     0
#define ENGINE1_RUNBIT   2
#define ENGINE1_FLTBIT   3
#define ENGINE2_RUNBIT   4
#define ENGINE2_FLTBIT   5
#define PUMP1_RUNBIT     6
#define PUMP1_FLTBIT     7
#define PUMP2_RUNBIT     8
#define PUMP2_FLTBIT     9
#define BACKFLOW1_RUNBIT 10
#define BACKFLOW1_FLTBIT 11
#define BACKFLOW2_RUNBIT 12
#define BACKFLOW2_FLTBIT 13
#define REUSE1_RUNBIT    14
#define REUSE1_FLTBIT    15
#define REUSE2_RUNBIT    16
#define REUSE2_FLTBIT    17
#define LEVEL1_HIGHBIT	 18
#define LEVEL1_LOWBIT    19
#define LEVEL2_HIGHBIT   20
#define LEVEL2_LOWBIT    21
#define LEVEL3_HIGHBIT	 22
#define LEVEL3_LOWBIT    23
#define LEVEL4_HIGHBIT	 24
#define LEVEL4_LOWBIT    25
#define VALUE1_OPENBIT   26
//#define VALUE1_FLTBIT    19
#define VALUE2_OPENBIT   27
//#define VALUE2_FLTBIT    21


#define ENGINE1_CONTROLBIT  	0
#define ENGINE2_CONTROLBIT  	1
#define PUMP1_CONTROLBIT    	2
#define PUMP2_CONTROLBIT    	3
#define BACKFLOW1_CONTROLBIT  	4
#define BACKFLOW2_CONTROLBIT  	5
#define REUSE1_CONTROLBIT   	6
#define REUSE2_CONTROLBIT   	7
#define VALUE1_CONTROLBIT   	8
#define VALUE2_CONTROLBIT   	9


//��õ�ǰ�Ĳ���ģʽ
INT8U Get_WorkType(void)
{
    if(DI_Val & ((INT32U)1<<WORKTYPE_BIT))
    {
		if(Sys_Param.worktype == WORKTYPE_AUTO)
			return WORKTYPE_AUTO; 
		else
			return WORKTYPE_REMOTE;
	}
	else
	{	
		return WORKTYPE_MANUAL;
	}
}

//��������õ�״̬
INT8U Get_PumpStat(INT8U Index)
{
    if((Sys_Param.pumpen & (1<<Index)) == 0)
       return DEVICE_DISABLE;
    
    switch(Index)
    {
    case 0:
        if(!(DI_Val & ((INT32U)1<<PUMP1_FLTBIT)))
            return DEVICE_FAULT; 
        else if(DI_Val & ((INT32U)1<<PUMP1_RUNBIT))
            return DEVICE_RUN;
        else
            return DEVICE_STOP;
    case 1:
        if(!(DI_Val & ((INT32U)1<<PUMP2_FLTBIT)))
            return DEVICE_FAULT; 
        else if(DI_Val & ((INT32U)1<<PUMP2_RUNBIT))
            return DEVICE_RUN;
        else
            return DEVICE_STOP;
    default:
        return DEVICE_DISABLE;
    }
}

//�����������״̬         
INT8U Get_EngineStat(INT8U Index)         
{
    if((Sys_Param.engineen & (1<<Index)) == 0)
       return DEVICE_DISABLE;
    
    switch(Index)
    {
    case 0:
        if(!(DI_Val & ((INT32U)1<<ENGINE1_FLTBIT)))
            return DEVICE_FAULT; 
        else if(DI_Val & ((INT32U)1<<ENGINE1_RUNBIT))
            return DEVICE_RUN;
        else
            return DEVICE_STOP;
    case 1:
        if(!(DI_Val & ((INT32U)1<<ENGINE2_FLTBIT)))
            return DEVICE_FAULT; 
        else if(DI_Val & ((INT32U)1<<ENGINE2_RUNBIT))
            return DEVICE_RUN;
        else
            return DEVICE_STOP;
    default:
        return DEVICE_DISABLE;
    }
}  
         
//��õ�ŷ���״̬
INT8U Get_ValueStat(INT8U Index)
{
    if((Sys_Param.valueen & (1<<Index)) == 0)
       return DEVICE_DISABLE;
    
    switch(Index)
    {
    case 0:
        /*if(!(DI_Val & ((INT32U)1<<VALUE1_FLTBIT)))
            return DEVICE_FAULT; 
        else if(DI_Val & ((INT32U)1<<VALUE1_OPENBIT))*/
		if(DI_Val & ((INT32U)1<<VALUE1_OPENBIT))
            return DEVICE_RUN;
		else 
            return DEVICE_STOP;
    case 1:
        /*if(!(DI_Val & ((INT32U)1<<VALUE2_FLTBIT)))
            return DEVICE_FAULT; 
        else if(DI_Val & ((INT32U)1<<VALUE2_OPENBIT))*/
		if(DI_Val & ((INT32U)1<<VALUE2_OPENBIT))
            return DEVICE_RUN;
		else 
            return DEVICE_STOP;
    default:
        return DEVICE_DISABLE;
    }
}

//��û����õ�״̬
INT8U Get_BackflowStat(INT8U Index)
{
    if((Sys_Param.backflowen & (1<<Index)) == 0)
       return DEVICE_DISABLE;
    
    switch(Index)
    {
    case 0:
        if(!(DI_Val & ((INT32U)1<<BACKFLOW1_FLTBIT)))
            return DEVICE_FAULT; 
        else if(DI_Val & ((INT32U)1<<BACKFLOW1_RUNBIT))
            return DEVICE_RUN;
        else
            return DEVICE_STOP;
    case 1:
        if(!(DI_Val & ((INT32U)1<<BACKFLOW2_FLTBIT)))
            return DEVICE_FAULT; 
        else if(DI_Val & ((INT32U)1<<BACKFLOW2_RUNBIT))
            return DEVICE_RUN;
        else
            return DEVICE_STOP;
    default:
        return DEVICE_DISABLE;
    }
}

//��û��ñõ�״̬
INT8U Get_ReuseStat(INT8U Index)
{
    if((Sys_Param.reuseen & (1<<Index)) == 0)
       return DEVICE_DISABLE;
    
    switch(Index)
    {
    case 0:
        if(!(DI_Val & ((INT32U)1<<REUSE1_FLTBIT)))
            return DEVICE_FAULT; 
        else if(DI_Val & ((INT32U)1<<REUSE1_RUNBIT))
            return DEVICE_RUN;
        else
            return DEVICE_STOP;
    case 1:
        if(!(DI_Val & ((INT32U)1<<REUSE2_FLTBIT)))
            return DEVICE_FAULT; 
        else if(DI_Val & ((INT32U)1<<REUSE2_RUNBIT))
            return DEVICE_RUN;
        else
            return DEVICE_STOP;
    default:
        return DEVICE_DISABLE;
    }
}

//���ˮλ��״̬����ˮλʱ���������������պϣ���ˮλʱ�������������Ͽ�
//�м�ˮλʱ��ֻ�е�ˮλ�������պ�
INT8U Get_LevelStat(INT8U Index)
{
    if((Sys_Param.levelen & (1<<Index)) == 0)
       return DEVICE_DISABLE;
	
	switch(Index)
    {
    case 0:
        if((DI_Val & ((INT32U)1<<LEVEL1_HIGHBIT)) && 
		   (DI_Val & ((INT32U)1<<LEVEL1_LOWBIT)))
			return LEVEL_HIGH;
		else if(DI_Val & ((INT32U)1<<LEVEL1_LOWBIT))
			return LEVEL_MID;
		else
			return LEVEL_LOW;
    case 1:
        if((DI_Val & ((INT32U)1<<LEVEL2_HIGHBIT)) && 
		   (DI_Val & ((INT32U)1<<LEVEL2_LOWBIT)))
			return LEVEL_HIGH;
		else if(DI_Val & ((INT32U)1<<LEVEL2_LOWBIT))
			return LEVEL_MID;
		else
			return LEVEL_LOW;
	case 2:
        if((DI_Val & ((INT32U)1<<LEVEL3_HIGHBIT)) && 
		   (DI_Val & ((INT32U)1<<LEVEL3_LOWBIT)))
			return LEVEL_HIGH;
		else if(DI_Val & ((INT32U)1<<LEVEL3_LOWBIT))
			return LEVEL_MID;
		else
			return LEVEL_LOW;
	case 3:
        if((DI_Val & ((INT32U)1<<LEVEL4_HIGHBIT)) && 
		   (DI_Val & ((INT32U)1<<LEVEL4_LOWBIT)))
			return LEVEL_HIGH;
		else if(DI_Val & ((INT32U)1<<LEVEL4_LOWBIT))
			return LEVEL_MID;
		else
			return LEVEL_LOW;
    default:
        return LEVEL_LOW;
    }
}

//����������
void Start_Pump(INT8U Index,INT8U Mutex)
{
    switch(Index)
    {
    case 0:
        if(Mutex == 1)
            DO_Val = (1<<PUMP1_CONTROLBIT);
        else
            DO_Val |= (1<<PUMP1_CONTROLBIT);
        break;
    case 1:
        if(Mutex == 1)
            DO_Val = (1<<PUMP2_CONTROLBIT);
        else
            DO_Val |= (1<<PUMP2_CONTROLBIT);
        break;
    default:
        break;
    }
}
//ֹͣ������
void Stop_Pump(INT8U Index)
{
    switch(Index)
    {
    case 0:
        DO_Val &= (~(1<<PUMP1_CONTROLBIT));
        break;
    case 1:
        DO_Val &= (~(1<<PUMP2_CONTROLBIT));
        break;
    default:
        break;
    }
}

//����������
void Start_Engine(INT8U Index,INT8U Mutex)
{
    switch(Index)
    {
    case 0:
        if(Mutex == 1)
            DO_Val = (1<<ENGINE1_CONTROLBIT);
        else
            DO_Val |= (1<<ENGINE1_CONTROLBIT);
        break;
    case 1:
        if(Mutex == 1)
            DO_Val = (1<<ENGINE2_CONTROLBIT);
        else
            DO_Val |= (1<<ENGINE2_CONTROLBIT);
        break;
    default:
        break;
    }
}
//ֹͣ������
void Stop_Engine(INT8U Index)
{
    switch(Index)
    {
    case 0:
        DO_Val &= (~(1<<ENGINE1_CONTROLBIT));
        break;
    case 1:
        DO_Val &= (~(1<<ENGINE2_CONTROLBIT));
        break;
   default:
        break;
    }
}
         
//������ŷ�
void Start_Value(INT8U Index,INT8U Mutex)
{
    switch(Index)
    {
    case 0:
		if(Mutex == 1)
            DO_Val = (1<<VALUE1_CONTROLBIT);
        else
            DO_Val |= (1<<VALUE1_CONTROLBIT);
		break;
    case 1:
		if(Mutex == 1)
            DO_Val = (1<<VALUE2_CONTROLBIT);
        else
            DO_Val |= (1<<VALUE2_CONTROLBIT);
		break;
    default:
        break;
    }
}
//ֹͣ��ŷ�
void Stop_Value(INT8U Index)
{
    switch(Index)
    {
    case 0:
        DO_Val &= (~(1<<VALUE1_CONTROLBIT));
		break;
    case 1:
        DO_Val &= (~(1<<VALUE2_CONTROLBIT));
        break;
    default:
        break;
    }
}

//����������
void Start_Backflow(INT8U Index,INT8U Mutex)
{
    switch(Index)
    {
    case 0:
        if(Mutex == 1)
            DO_Val = (1<<BACKFLOW1_CONTROLBIT);
        else
            DO_Val |= (1<<BACKFLOW1_CONTROLBIT);
        break;
    case 1:
        if(Mutex == 1)
            DO_Val = (1<<BACKFLOW2_CONTROLBIT);
        else
            DO_Val |= (1<<BACKFLOW2_CONTROLBIT);
        break;
    default:
        break;
    }
}
//ֹͣ������
void Stop_Backflow(INT8U Index)
{
    switch(Index)
    {
    case 0:
        DO_Val &= (~(1<<BACKFLOW1_CONTROLBIT));
        break;
    case 1:
        DO_Val &= (~(1<<BACKFLOW2_CONTROLBIT));
        break;
    default:
        break;
    }
}

//�������ñ�
void Start_Reuse(INT8U Index,INT8U Mutex)
{
    switch(Index)
    {
    case 0:
        if(Mutex == 1)
            DO_Val = (1<<REUSE1_CONTROLBIT);
        else
            DO_Val |= (1<<REUSE1_CONTROLBIT);
        break;
    case 1:
        if(Mutex == 1)
            DO_Val = (1<<REUSE2_CONTROLBIT);
        else
            DO_Val |= (1<<REUSE2_CONTROLBIT);
        break;
    default:
        break;
    }
}
//ֹͣ���ñ�
void Stop_Reuse(INT8U Index)
{
    switch(Index)
    {
    case 0:
        DO_Val &= (~(1<<REUSE1_CONTROLBIT));
        break;
    case 1:
        DO_Val &= (~(1<<REUSE2_CONTROLBIT));
        break;
    default:
        break;
    }
}

void Stop_All()
{
    DO_Val = 0;   
}

//��������õĿ���״̬
INT8U Get_PumpControlStat(INT8U Index)
{
    if((Sys_Param.pumpen & (1<<Index)) == 0)
       return DEVICE_DISABLE;
    
    switch(Index)
    {
    case 0:
        if(DO_Val & (1<<PUMP1_CONTROLBIT))
            return DEVICE_RUN; 
        else
            return DEVICE_STOP;
    case 1:
        if(DO_Val & (1<<PUMP2_CONTROLBIT))
            return DEVICE_RUN; 
        else
            return DEVICE_STOP;
    default:
        return DEVICE_DISABLE;
    }
}

//����������Ŀ���״̬
INT8U Get_EngineControlStat(INT8U Index)
{
    if((Sys_Param.engineen & (1<<Index)) == 0)
       return DEVICE_DISABLE;
    
    switch(Index)
    {
    case 0:
        if(DO_Val & (1<<ENGINE1_CONTROLBIT))
            return DEVICE_RUN; 
        else
            return DEVICE_STOP;
    case 1:
        if(DO_Val & (1<<ENGINE2_CONTROLBIT))
            return DEVICE_RUN; 
        else
            return DEVICE_STOP;
    default:
        return DEVICE_DISABLE;
    }
}

//��õ�ŷ��Ŀ���״̬
INT8U Get_ValueControlStat(INT8U Index)
{
    if((Sys_Param.valueen & (1<<Index)) == 0)
       return DEVICE_DISABLE;
    
    switch(Index)
    {
    case 0:
        if(DO_Val & (1<<VALUE1_CONTROLBIT))
            return DEVICE_RUN; 
        else 
            return DEVICE_STOP; 
	case 1:
        if(DO_Val & (1<<VALUE2_CONTROLBIT))
            return DEVICE_RUN; 
        else 
            return DEVICE_STOP; 
	default:
        return DEVICE_DISABLE;
    }
}

//��û����õĿ���״̬
INT8U Get_BackflowControlStat(INT8U Index)
{
    if((Sys_Param.backflowen & (1<<Index)) == 0)
       return DEVICE_DISABLE;
    
    switch(Index)
    {
    case 0:
        if(DO_Val & (1<<BACKFLOW1_CONTROLBIT))
            return DEVICE_RUN; 
        else
            return DEVICE_STOP;
    case 1:
        if(DO_Val & (1<<BACKFLOW2_CONTROLBIT))
            return DEVICE_RUN; 
        else
            return DEVICE_STOP;
    default:
        return DEVICE_DISABLE;
    }
}

//��û��ñõĿ���״̬
INT8U Get_ReuseControlStat(INT8U Index)
{
    if((Sys_Param.reuseen & (1<<Index)) == 0)
       return DEVICE_DISABLE;
    
    switch(Index)
    {
    case 0:
        if(DO_Val & (1<<REUSE1_CONTROLBIT))
            return DEVICE_RUN; 
        else
            return DEVICE_STOP;
    case 1:
        if(DO_Val & (1<<REUSE2_CONTROLBIT))
            return DEVICE_RUN; 
        else
            return DEVICE_STOP;
    default:
        return DEVICE_DISABLE;
    }
}

//�Զ�����������
void Start_PumpAuto(void)
{
    static char PumpIndex = 0; 
    char sflag = 0;
    char i;
    
    for(i=0;i<2;++i)
    {
        //�Ѿ����豸���ڴ�״̬
        if((Get_PumpStat(i) != DEVICE_DISABLE) &&
            (Get_PumpStat(i) != DEVICE_FAULT) &&
            (Get_PumpControlStat(i) == DEVICE_RUN))
        {
            PumpIndex = i;
            return;
        }
    }
    
    for(i=0;i<2;++i)
    {
        //Ѱ��һ�������豸������
        if(i != PumpIndex)
        {    
            if((Get_PumpStat(i) != DEVICE_DISABLE) &&
                (Get_PumpStat(i) != DEVICE_FAULT))
            {
                Start_Pump(i,0);
                PumpIndex = i;
                sflag = 1;
                break;
            }
        }
    }
    
    //û�ҵ������豸�������Լ�
    if(sflag == 0)
    {
        if((Get_PumpStat(PumpIndex) != DEVICE_DISABLE) &&
            (Get_PumpStat(PumpIndex) != DEVICE_FAULT))
        {
            Start_Pump(PumpIndex,0);
            sflag = 1;
        }   
    }
}
//�Զ�ֹͣ������
void Stop_PumpAuto(void)
{
    char i;
    for(i=0;i<2;++i)   
        Stop_Pump(i); 
    //Stop_All();
}

//�Զ�����������
void Start_EngineAuto(int IsSwitch)
{
    static char EngineIndex = 0; 
    char sflag = 0;
	char i;
    
	if(0 == IsSwitch)	//�Ƿ��л�������
	{
		for(i=0;i<2;++i)
		{
			//�Ѿ����豸���ڴ�״̬
			if((Get_EngineStat(i) != DEVICE_DISABLE) &&
				(Get_EngineStat(i) != DEVICE_FAULT) &&
				(Get_EngineControlStat(i) == DEVICE_RUN))
			{
				EngineIndex = i;
				sflag = 1;
			}
		}
		
		if(sflag == 0)
		{
			for(i=0;i<2;++i)
			{
				if(i != EngineIndex)
				{    
					if((Get_EngineStat(i) != DEVICE_DISABLE) &&
						(Get_EngineStat(i) != DEVICE_FAULT))
					{
						Start_Engine(i,0);
						EngineIndex = i;
						sflag = 1;
						break;
					}
				}
			}
		}
		
		if(sflag == 0)
		{
			if((Get_EngineStat(EngineIndex) != DEVICE_DISABLE) &&
				(Get_EngineStat(EngineIndex) != DEVICE_FAULT))
			{
				Start_Engine(EngineIndex,0);
				sflag = 1;
			}   
		}
	}
	else
	{
		for(i=0;i<2;++i)
		{
			//�Ѿ����豸���ڴ�״̬
			if((Get_EngineStat(i) != DEVICE_DISABLE) &&
				(Get_EngineStat(i) != DEVICE_FAULT) &&
				(Get_EngineControlStat(i) == DEVICE_RUN))
			{
				EngineIndex = i;
				break;
			}
		}
		
		for(i=0;i<2;++i)
		{
			if(i != EngineIndex)
			{    
				if((Get_EngineStat(i) != DEVICE_DISABLE) &&
					(Get_EngineStat(i) != DEVICE_FAULT))
				{
					Start_Engine(i,0);
					EngineIndex = i;
					sflag = 1;
					break;
				}
			}
		}
		
		if(sflag == 0)
		{
			if((Get_EngineStat(EngineIndex) != DEVICE_DISABLE) &&
				(Get_EngineStat(EngineIndex) != DEVICE_FAULT))
			{
				Start_Engine(EngineIndex,0);
				sflag = 1;
			}   
		}
	}
	
	for(i=0;i<2;++i)
	{
		if(i != EngineIndex)
		{    
			Stop_Engine(i);
		}
	}
}
void Start_AllEngine(void)
{
	char i;
	
	for(i=0;i<2;++i)
    {
        if((Get_EngineStat(i) != DEVICE_DISABLE) &&
			(Get_EngineStat(i) != DEVICE_FAULT))
		{
			Start_Engine(i,0);	//����������
		}
    }
}
//�Զ�ֹͣ������
void Stop_EngineAuto(void)
{
    char i;
    for(i=0;i<2;++i)   
        Stop_Engine(i); 
    //Stop_All();
}

//�Զ�������ŷ�
void Start_ValueAuto(void)
{
    char i;
	
	for(i=0;i<2;++i)
    {
        if((Get_ValueStat(i) != DEVICE_DISABLE) &&
			(Get_ValueStat(i) != DEVICE_FAULT))
		{
			Start_Value(i,0);	//����������
		}
    }
}
//�Զ�ֹͣ��ŷ�
void Stop_ValueAuto(void)
{
    char i;
    for(i=0;i<2;++i)   
        Stop_Value(i); 
    //Stop_All();
}

//�Զ�����������
void Start_BackflowAuto(int Index)
{
    if((Get_BackflowStat(Index) != DEVICE_DISABLE) &&
            (Get_BackflowStat(Index) != DEVICE_FAULT))
	{
		Start_Backflow(Index,0);
	}
}
//�Զ�ֹͣ������
void Stop_BackflowAuto(int Index)
{
    Stop_Backflow(Index); 
    //Stop_All();
}

//�Զ���������������
void Start_ReuseAuto(void)
{
    static char ReuseIndex = 0; 
    char sflag = 0;
    char i;
    
    for(i=0;i<2;++i)
    {
        //�Ѿ����豸���ڴ�״̬
        if((Get_ReuseStat(i) != DEVICE_DISABLE) &&
            (Get_ReuseStat(i) != DEVICE_FAULT) &&
            (Get_ReuseControlStat(i) == DEVICE_RUN))
        {
            ReuseIndex = i;
            return;
        }
    }
    
    for(i=0;i<2;++i)
    {
        //Ѱ��һ�������豸������
        if(i != ReuseIndex)
        {    
            if((Get_ReuseStat(i) != DEVICE_DISABLE) &&
                (Get_ReuseStat(i) != DEVICE_FAULT))
            {
                Start_Reuse(i,0);
                ReuseIndex = i;
                sflag = 1;
                break;
            }
        }
    }
    
    //û�ҵ������豸�������Լ�
    if(sflag == 0)
    {
        if((Get_ReuseStat(ReuseIndex) != DEVICE_DISABLE) &&
            (Get_ReuseStat(ReuseIndex) != DEVICE_FAULT))
        {
            Start_Reuse(ReuseIndex,0);
            sflag = 1;
        }   
    }
}
//�Զ�ֹͣ���ñ�
void Stop_ReuseAuto(void)
{
    char i;
    for(i=0;i<2;++i)   
        Stop_Reuse(i); 
    //Stop_All();
}
