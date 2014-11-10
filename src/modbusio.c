

#include "system.h"
#include "modbusio.h"
#include "sc16is752.h"
#include "dio.h"
#include <stdlib.h>
#include <string.h>


MBIO_PARAM MbIO_Param;

extern const char ComBaud_Str[4][6];
char MbIO_Err[MODBUSIO_VARNUM];

//=====================================
//TimerA�Ķ�ʱ��10ms����һ���ж�
//��ÿ���ж��м���һ����������
//ÿ10ms��������1������modbus�ĳ�ʱ�ж�
//=====================================
#define MBIOOTCNTNUM (MS2TENMS(1000)) 	//modbus��ʱֵ�ļ�����
#define MBIODELAYCNTNUM (MS2TENMS(150)) //����modbus�豸֮�����ʱ
INT8U MbIOOtCnt;			        	//modbus��ʱ������
INT8U MbIODelayCnt;

#define  MBIOMAXERRCNT 3		    	//modbus�����������������ֵ��Ὣ��������
//=====================================
//ModbusͨѶ���õ���ȫ�ֱ���
//=====================================
static unsigned char MBIOCurrentDev;		//modbus��ǰ��������豸���
static unsigned char MBIOState;			//modbus��ǰ�Ĵ���״̬
static unsigned char MBIORcvDataLen;		//modbusҪ���յ�Ӧ�����ݳ���
static unsigned char MBIORcvDataBuf[32];	//modbus���յ�Ӧ�����ݴ�ŵĻ�����
static unsigned char MBIORcved;			//modbus�ѽ��յ�Ӧ�����ݳ���

//=====================================
//����IO��Ĭ�ϲɼ�����
//=====================================
void Default_MbIOParam(MBIO_PARAM *Param)
{
    Param->baud_index = 0;
	
	Param->devid[0] = 1;
	Param->funccode[0] = 2;
	Param->regaddr[0] = 1;
	Param->reglen[0] = 16;
	
	Param->devid[1] = 2;
	Param->funccode[1] = 15;
	Param->regaddr[1] = 17;
	Param->reglen[1] = 8;
}
//=====================================
//modbus��ʼ��
//=====================================
void ModbusIO_Init(void)
{
	Default_MbIOParam(&MbIO_Param);
	IS752_ChangeBaudRate(atoi(ComBaud_Str[MbIO_Param.baud_index]),MODBUSIO_COM);
}

//=====================================
//modbus crc����
//=====================================
extern unsigned short CRC16_Modbus(unsigned char *src, int len);

//=====================================
//modbus �����
//=====================================
void SendModbusIOCmd(void)
{
	unsigned int regaddr, reglen;
	unsigned char devaddr, funccode;
	int uartchannel;
	unsigned char modbus_bag[22];
	unsigned short crc;
	unsigned char len, doval;
	
	uartchannel = MODBUSIO_COM;
	devaddr = MbIO_Param.devid[MBIOCurrentDev];
	funccode = MbIO_Param.funccode[MBIOCurrentDev]; 
	regaddr = MbIO_Param.regaddr[MBIOCurrentDev] - 1;
	reglen = MbIO_Param.reglen[MBIOCurrentDev];
	
	if(funccode == 2)
	{
		//�豸��ֵ
		modbus_bag[0] = devaddr;
		//�����
		modbus_bag[1] = funccode;
		//modbus��ʼ��ַ
		//��λ��ַ
		modbus_bag[2] = (regaddr&0xFF00)>>8;
		//��λ��ַ
		modbus_bag[3] = regaddr&0x00FF;
		//�ֽڸ���
		modbus_bag[4] = (reglen&0xFF00)>>8;
		modbus_bag[5] = reglen&0x00FF;
		//���ݶ�У����
		crc = CRC16_Modbus(modbus_bag,6);
		modbus_bag[6] = (crc&0xFF00)>>8;	//���ֽ�
		modbus_bag[7] = crc&0x00FF;	//���ֽ�
		len = 8;
		
		if(reglen%8 == 0)
			MBIORcvDataLen = 5 + reglen/8;
		else
			MBIORcvDataLen = 5 + reglen/8 + 1;
	}
	else// if(funccode == 15)
	{
		//�豸��ֵ
		modbus_bag[0] = devaddr;
		//�����
		modbus_bag[1] = funccode;
		//modbus��ʼ��ַ
		//��λ��ַ
		modbus_bag[2] = (regaddr&0xFF00)>>8;
		//��λ��ַ
		modbus_bag[3] = regaddr&0x00FF;
		//�ֽڸ���
		modbus_bag[4] = (reglen&0xFF00)>>8;
		modbus_bag[5] = reglen&0x00FF;
		//�����ֽ�����Ŀǰ�̶�Ϊ1
		modbus_bag[6] = 1;
		//IO���������
		doval = (unsigned char)(DO_Val>>8);
		modbus_bag[7] = doval;
		//���ݶ�У����
		crc = CRC16_Modbus(modbus_bag,8);
		modbus_bag[8] = (crc&0xFF00)>>8;	//���ֽ�
		modbus_bag[9] = crc&0x00FF;	//���ֽ�
		len = 10;
		
		MBIORcvDataLen = 8;
	}
	
	//����Э���
	Uart_ClearRcvBuf(uartchannel);
	Uart_SendData(modbus_bag, len, uartchannel);
	
	MbIOOtCnt = 0;
	MBIORcved = 0;
}
//=====================================
//modbus ����Ӧ����պ���, ����-1��ʾ���ճ�ʱ��0��ʾ�������գ�1��ʾ�������
//=====================================
int RcvModbusIORsp(void)
{
	int ret, uartchannel;
	
	uartchannel = MODBUSIO_COM;

	while(1)
	{
		ret = Uart_RcvData(&MBIORcvDataBuf[MBIORcved],1,uartchannel);
		if(ret < 1)
		{
			if (MbIOOtCnt >= MBIOOTCNTNUM)
				return -1;
			else
				return 0;
		}
		
		if (++MBIORcved >= MBIORcvDataLen)	//���ݽ������
			return 1;
	}
}
//=====================================
//modbus ����Ӧ���������, ����-1��ʾ���ݴ���0��ʾ��������ȷ����
//=====================================
int AnalystModbusIORsp(void)
{
	unsigned char devaddr, funccode;
	unsigned short crc, crc2;
    INT16U dival;
		
    devaddr = MbIO_Param.devid[MBIOCurrentDev];
	funccode = MbIO_Param.funccode[MBIOCurrentDev];
	
	if (MBIORcvDataBuf[0] != devaddr)
		return -1;
	
	if (MBIORcvDataBuf[1] != funccode)
		return -1;
	
	crc = CRC16_Modbus(MBIORcvDataBuf, MBIORcvDataLen-2);
	crc2 = (MBIORcvDataBuf[MBIORcvDataLen-2]<<8) + MBIORcvDataBuf[MBIORcvDataLen-1];
	if(crc != crc2)
		return -1;
	
	if(funccode == 2)
	{
		if(MBIORcvDataBuf[2] == 1)
			dival = MBIORcvDataBuf[3];
		else
			dival = MBIORcvDataBuf[3] + ((INT16U)MBIORcvDataBuf[4]<<8);
		DI_Val = (DI_Val&0x0000ffff) + ((INT32U)(dival)<<16);
	}
	else// if(funccode == 15)
	{
	}
	
	MbIO_Err[MBIOCurrentDev] = 0;
	
	return 0;
}
//=====================================
//modbus �������
//=====================================
void ProcessModbusIO(void)
{
	int flag, ret;

	flag = 0;
	while(1)
	{
		switch(MBIOState)
		{
		case 0:		//��������
            {
				if(++MbIO_Err[MBIOCurrentDev] > MBIOMAXERRCNT)
                {
                    MbIO_Err[MBIOCurrentDev] = 0;
                }
                
                SendModbusIOCmd();
                MBIOState = 1;
                flag = 1;
			}
            break;
		case 1:		//����Ӧ��
			ret = RcvModbusIORsp();
			if(0 == ret)
			{
				flag = 1;
			}
			else if(-1 == ret)
			{
				MBIOState = 3;
			}
			else
			{
				MBIOState = 2;
			}
			break;
		case 2:		//����Ӧ��
			ret = AnalystModbusIORsp();
			if(-1 == ret)
			{
			}
			MBIOState = 3;
			MbIODelayCnt = 0;
			break;
		case 3:		//�л��豸
			++MBIOCurrentDev;
			if(MBIOCurrentDev < MODBUSIO_VARNUM)	//����ɨ����һ���豸
			{
				MBIOState = 0xff;
				flag = 1;
			}
			else						//�ȴ�ֱ��һ��ɨ�����ڽ����ٽ�����һ��ɨ������
			{
				MBIOCurrentDev = 0;
				MBIOState = 0xff;
				flag = 1;
			}
			break;
		case 0xff:
			{
				if(MbIODelayCnt >= MBIODELAYCNTNUM)
					MBIOState = 0;
				flag = 1;	
			}
			break;
		default:
			flag = 1;
			break;
		}
		
		if(1 == flag)
			break;
	}
}

