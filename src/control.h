


#ifndef _CONTROL_H
#define _CONTROL_H

typedef struct _SWITCH_TIME
{
	INT8U starthour;	//����ʱ��
	INT8U startmin;
} SWITCH_TIME;

typedef struct _SYS_PARAM
{
    char pw[8];
    char st[4];
    char mn[16];
    char sim[12];
    char rtd[4];            //ʵʱ�����ϱ����
    
    INT8U worktype;         //�Ƿ���������ʽ��Զ�̿���
    INT8U pumpen;           //�������Ƿ�ʹ��
    INT8U engineen;         //�������Ƿ�ʹ��
    INT8U valueen;          //��ŷ��Ƿ�ʹ��
	INT8U backflowen;       //����������Ƿ�ʹ��
	INT8U reuseen;          //���ñ��Ƿ�ʹ��
	INT8U levelen;          //Һλ�Ƿ�ʹ��
	
	float dohigh;			//����رյ�do��ֵ
	float dolow;			//���������do��ֵ
	INT16U engine_switchlong;	//�������л�ʱ����
	INT16U value_switchlong;	//��ŷ��л�ʱ����
	INT16U value_switchlast;	//��ŷ��л�ʱ��
} SYS_PARAM;


extern SYS_PARAM Sys_Param;

void ProcControl(void);
void ControlInit(void);

/////////////////////////////
#define WORKTYPE_AUTO   0
#define WORKTYPE_MANUAL 1
#define WORKTYPE_REMOTE 2

#define DEVICE_STOP     0
#define DEVICE_RUN      1
#define DEVICE_FAULT    2
#define DEVICE_DISABLE  3

#define LEVEL_HIGH      0
#define LEVEL_MID       1
#define LEVEL_LOW       2


//void ClearAllControlTimer(void);

INT8U Get_WorkType(void);
INT8U Get_PumpStat(INT8U Index);
INT8U Get_EngineStat(INT8U Index);
INT8U Get_ValueStat(INT8U Index);
INT8U Get_BackflowStat(INT8U Index);
INT8U Get_ReuseStat(INT8U Index);
INT8U Get_LevelStat(INT8U Index);

void Start_Pump(INT8U Index,INT8U Mutex);
void Stop_Pump(INT8U Index);
void Start_Engine(INT8U Index,INT8U Mutex);
void Stop_Engine(INT8U Index);
void Start_Value(INT8U Index,INT8U Mutex);
void Stop_Value(INT8U Index);
void Start_Backflow(INT8U Index,INT8U Mutex);
void Stop_Backflow(INT8U Index);
void Start_Reuse(INT8U Index,INT8U Mutex);
void Stop_Reuse(INT8U Index);

INT8U Get_PumpControlStat(INT8U Index);
INT8U Get_EngineControlStat(INT8U Index);
INT8U Get_ValueControlStat(INT8U Index);
INT8U Get_BackflowControlStat(INT8U Index);
INT8U Get_ReuseControlStat(INT8U Index);

void Start_PumpAuto(void);
void Stop_PumpAuto(void);
void Start_EngineAuto(int IsSwitch);
void Start_AllEngine(void);
void Stop_EngineAuto(void);
void Start_ValueAuto(void);
void Stop_ValueAuto(void);
void Start_BackflowAuto(int Index);
void Stop_BackflowAuto(int Index);
void Start_ReuseAuto(void);
void Stop_ReuseAuto(void);

#endif //end of _CONTROL_H
