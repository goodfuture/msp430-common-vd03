

#include <string.h>
#include "system.h"
#include "lcd.h"
#include "zimo.h"


#define LCD_RST_DIS P9OUT|=BIT3
#define LCD_RST_EN  P9OUT&=~BIT3

#define LCD_WR_DIS  P9OUT|=BIT7
#define LCD_WR_EN   P9OUT&=~BIT7

#define LCD_RD_DIS  P9OUT|=BIT6
#define LCD_RD_EN   P9OUT&=~BIT6

#define LCD_CE_DIS  P9OUT|=BIT5
#define LCD_CE_EN   P9OUT&=~BIT5

#define LCD_CD_CMD  P9OUT|=BIT4
#define LCD_CD_DAT  P9OUT&=~BIT4

#define LCD_WR_DAT(X)  P10OUT=X
#define LCD_RD_DAT(X)  P10DIR&=0X00;P5OUT|=BIT0;X=P10IN;P5OUT&=~BIT0;P10DIR|=0XFF

#define LCD_CMD_SETCURPOT   0x21
#define LCD_CMD_SETOFTREG   0x22
#define LCD_CMD_SETADDRPOT  0x24
#define LCD_CMD_SETTXTADDR  0x40
#define LCD_CMD_SETTXTAERA  0x41
#define LCD_CMD_SETIMGADDR  0x42
#define LCD_CMD_SETIMGAERA  0x43
#define LCD_CMD_SETORMODE   0x80
#define LCD_CMD_SETEXORMODE 0x81
#define LCD_CMD_SETANDMODE  0x83
#define LCD_CMD_SETTXTMODE  0x84
#define LCD_CMD_SETDISOFF   0x90
#define LCD_CMD_SETCONBOFF  0x92
#define LCD_CMD_SETCONBON   0x93
#define LCD_CMD_SETTONGOFF  0x94
#define LCD_CMD_SETTOFFGON  0x98
#define LCD_CMD_SETTONGON   0x9C
#define LCD_CMD_SETAUTOWR   0xB0
#define LCD_CMD_SETAUTORD   0xB1
#define LCD_CMD_SETAUTORST  0xB2
#define LCD_CMD_INCWRDATE   0xC0


//��״̬
void chk_busy(INT8U autowr)    
{
    INT8U inbuf;
	INT8U cnt;
    
    LCD_WR_DAT(0xff);
    LCD_CD_CMD; //cd=1;
    LCD_WR_DIS; //wr=1;
    LCD_RD_EN;  //rd=0;
    LCD_RD_DAT(inbuf);
    
    if(autowr)
    {
        cnt = 0;
		while((inbuf&0x08)==0)
		{
            LCD_RD_DAT(inbuf);
			if(++cnt > 100)
				break;
		}
    }
    else
    {
        cnt = 0;
		while(((inbuf&0x01)==0)||((inbuf&0x02)==0))
		{
            LCD_RD_DAT(inbuf);
			if(++cnt > 100)
				break;
		}
    }
    
    LCD_RD_DIS; //rd=1;
}

//д����
void wr_comm (INT8U comm)       
{
    chk_busy(0);
    LCD_CD_CMD;   //cd=1;
    LCD_RD_DIS;   //rd=1;
    LCD_WR_DAT(comm);
    LCD_WR_EN;    //wr=0;
    //Delay_10_uS();
    LCD_WR_DIS;   // wr=1;
}

//д����
void wr_data (INT8U dat)       
{
    chk_busy(0);
    LCD_CD_DAT;   //cd=0;
    LCD_RD_DIS;   //rd=1;
    LCD_WR_DAT(dat);
    LCD_WR_EN;    //wr=0;
    //Delay_10_uS();
    LCD_WR_DIS;   //wr=1;
}

//дһ�����ݺ�һ������
void wr_od(INT8U dat,INT8U comm)       
{
    wr_data(dat);
    wr_comm(comm);
}

//д�������ݺ�һ������
void wr_td(INT8U datl,INT8U dath,INT8U comm)  
{
    wr_data(datl);
    wr_data(dath);
    wr_comm(comm);
}

//дһ��16�������ݺ�һ������
void wr_xd(INT16U dat,INT8U comm)       
{
    INT8U datl,dath;
    
    datl=dat;
    dath=dat>>8;
    wr_data(datl);
    wr_data(dath);
    wr_comm(comm);
}

//�Զ�д����
void wr_auto (INT8U dat)               
{
    chk_busy (1);
    LCD_CD_DAT;     //cd=0;
    LCD_RD_DIS;     //rd=1;
    LCD_WR_DAT(dat);
    LCD_WR_EN;      //wr=0;
    //Delay_10_uS();
    LCD_WR_DIS;     // wr=1;
}

//��ʼ��LCD
void LCD_Init(void)
{
    LCD_RST_EN;     //rst=0;
    Delay_10_uS();
    LCD_RST_DIS;    //rst=1;
    
    LCD_CE_EN;      //ce=0;
    LCD_WR_DIS;     //wr=1;
    LCD_RD_DIS;     //rd=1;
    
    wr_xd(LCD_TXT_ADDR,LCD_CMD_SETTXTADDR);     //�ı���ʾ���׵�ַ
    wr_xd(LCD_IMG_ADDR,LCD_CMD_SETIMGADDR);     //ͼ����ʾ���׵�ַ
    wr_td(LCD_WIDTH,0x00,LCD_CMD_SETTXTAERA);   //�ı���ʾ�����
    wr_td(LCD_WIDTH,0x00,LCD_CMD_SETIMGAERA);   //ͼ����ʾ�����
    wr_comm(LCD_CMD_SETEXORMODE);               //�߼�"���"
    wr_td(0x02,0x00,LCD_CMD_SETOFTREG);         //CGRAMƫ�õ�ַ����
    wr_comm(LCD_CMD_SETTOFFGON);                //����ͼ����ʾ
}

//����
void Clr_LCDRam(void)
{
    INT8U i,j,cnt;
    INT8U inbuf;
    
    wr_xd(LCD_IMG_ADDR,LCD_CMD_SETADDRPOT);
    //------------------------------------
    LCD_WR_DAT(0xff);
    LCD_CD_CMD;     //cd=1;
    LCD_WR_DIS;     //wr=1;
    LCD_RD_EN;      //rd=0;
    LCD_RD_DAT(inbuf);
	cnt = 0;
    while(1)
    {
        LCD_RD_DAT(inbuf);   
        if((inbuf&0x03)==0x03)
        {
            wr_comm(LCD_CMD_SETAUTOWR);
            break;
        }
		if(++cnt > 100)
			break;
    }
    LCD_RD_DIS;     // rd=1;
    
    //------------------------------------
    for(j=0;j<128;j++)
    {
        for(i=0;i<LCD_WIDTH;i++)
            wr_auto(0x00);
    }
    LCD_WR_DAT(0xff);
    LCD_CD_CMD;     //cd=1;
    LCD_WR_DIS;     //wr=1;
    LCD_RD_EN;      //rd=0;
    LCD_RD_DAT(inbuf);
	cnt = 0;
    while(1)
    {
        LCD_RD_DAT(inbuf);
        if((inbuf&0x08)==0x08)
        {
            wr_comm(LCD_CMD_SETAUTORST);
            break;
        }
		if(++cnt > 100)
			break;
    }
    LCD_RD_DIS;     //rd=1;
}

//���ָ�������N���ַ�
void Clr_RamBlock(INT8U X0,INT8U Y0,INT8U nbyte)
{
  	INT8U j,k;
	INT16U addr;
	for(k=0;k<nbyte;k++)
  	{
		addr=LCD_IMG_ADDR+X0+Y0*LCD_WIDTH+k;
	    for(j=0;j<16;j++)
		{
			wr_xd(addr+j*LCD_WIDTH,LCD_CMD_SETADDRPOT);
            wr_od(0x00,LCD_CMD_INCWRDATE);		
		}
	}
}

//��ʾͼ���ļ�
void Disp_Img(INT16U addr,INT8U xl,INT8U yl,const INT8U *img)
{
    INT8U i,j;
    for(j=0;j<yl;j++)
    {
        for(i=0;i<xl;i++)
        {
            wr_xd(addr+j*LCD_WIDTH+i,LCD_CMD_SETADDRPOT);
            wr_od(img[j*xl+i],LCD_CMD_INCWRDATE);
        }
    }
}

//��ʾһ���ַ�16*16��Index���ַ���������zimo.h)��PonΪ����
void Disp_Chn1616_Char(INT8U X0,INT8U Y0,INT8U Index,INT8U PoN)
{
    INT8U i,j;
    INT16U addr;
    
    addr=LCD_IMG_ADDR+X0+Y0*LCD_WIDTH;
    for(j=0;j<16;j++)
    {
        for(i=0;i<2;i++)
        {
            wr_xd(addr+j*LCD_WIDTH+i,LCD_CMD_SETADDRPOT);
            if(PoN) 
                wr_od(zimo1616[Index][i+2*j],LCD_CMD_INCWRDATE);
            else    
                wr_od(~zimo1616[Index][i+2*j],LCD_CMD_INCWRDATE);
        }
    }
}

//��ʾһ���ַ�16*16��Index���ַ���������zimo.h)��PonΪ����
void Disp_Chn1616_Str(INT8U X0,INT8U Y0,const INT8U *Index,INT8U PoN)
{
	INT8U i,j,k,m;
	INT16U addr;
    
    m = strlen((char *)Index);
    for(k=0;k<m;k++)
    {
        addr=LCD_IMG_ADDR+X0+Y0*LCD_WIDTH+k*2;
        for(j=0;j<16;j++)
        {
            for(i=0;i<2;i++)
            {
                wr_xd(addr+j*LCD_WIDTH+i,LCD_CMD_SETADDRPOT);
                if(PoN) 
                    wr_od(zimo1616[*Index-1][i+2*j],LCD_CMD_INCWRDATE);
                else    
                    wr_od(~zimo1616[*Index-1][i+2*j],LCD_CMD_INCWRDATE);
            }
        }
        Index++;
    }
}

//��ʾһ���ַ�8*16��Index���ַ���������zimo.h)��PonΪ����
void Disp_Chn816_Char(INT8U X0,INT8U Y0,INT8U Index,INT8U PoN) 
{
	INT8U j;
	INT16U addr;
    
	addr=LCD_IMG_ADDR+X0+Y0*LCD_WIDTH;
    for(j=0;j<16;j++)
	{
        wr_xd(addr+j*LCD_WIDTH,LCD_CMD_SETADDRPOT);
        if(PoN) 
            wr_od(zimo816[Index][j],LCD_CMD_INCWRDATE);
        else    
            wr_od(~zimo816[Index][j],LCD_CMD_INCWRDATE);			
	}
}

//��ʾһ���ַ�16*16��Index���ַ���������zimo.h)��PonΪ����
void Disp_Chn816_Str(INT8U X0,INT8U Y0,const INT8U *Index,INT8U PoN) 
{
	INT8U j,k,m;
	INT16U addr;
    
	m=strlen((char *)Index);
    for(k=0;k<m;k++)
    {
	    addr=LCD_IMG_ADDR+X0+Y0*LCD_WIDTH+k;
        for(j=0;j<16;j++)
	    {
            wr_xd(addr+j*LCD_WIDTH,LCD_CMD_SETADDRPOT);
            if(PoN) 
                wr_od(zimo816[*Index-1][j],LCD_CMD_INCWRDATE);
            else    
                wr_od(~zimo816[*Index-1][j],LCD_CMD_INCWRDATE);
	    }
        Index++;
    }
}

//��ʾ����
void Disp_Yline(INT8U X0,INT8U Y0)
{
	INT8U a=0x10,i;
	INT16U addr;
    
	addr=LCD_IMG_ADDR+X0;
	for(i=Y0;i<128;i++)
	{
        wr_xd(addr+i*LCD_WIDTH,LCD_CMD_SETADDRPOT);
        wr_od(a,LCD_CMD_INCWRDATE);
	}
}

//��ʾ����
void Disp_Xline(INT8U Y0)
{
    INT8U a=0xff,i;
    INT16U addr;
    addr=LCD_IMG_ADDR+Y0*LCD_WIDTH;
    for(i=0;i<LCD_WIDTH;i++)
    {
        wr_xd(addr+i,LCD_CMD_SETADDRPOT);
        wr_od(a,LCD_CMD_INCWRDATE);
    }
}

///////////////////////////////////////////////////////////////
/////���̲���
/////////////////////////////////////////////////////////////

#define KEYREAD() P8IN|0XC0


#define KEY_UP_MASK		0xFD
#define KEY_DOWN_MASK	0xfb
#define KEY_LEFT_MASK	0xf7
#define KEY_RIGHT_MASK	0xEF
#define KEY_ENTER_MASK 	0xDF
#define KEY_CANCEL_MASK	0xFE
#define KEY_RESET_MASK  KEY_CANCEL_MASK&KEY_DOWN_MASK&KEY_LEFT_MASK&KEY_ENTER_MASK//0xE8
#define LCD_RESET_MASK  KEY_ENTER_MASK&KEY_CANCEL_MASK     //0xEE		
#define BACK_DOOR0_MASK KEY_LEFT_MASK&KEY_RIGHT_MASK       //0xDD
#define BACK_DOOR1_MASK KEY_LEFT_MASK&KEY_RIGHT_MASK&KEY_CANCEL_MASK   //0xCD

INT8U KeyTimer;

INT8U Get_KeyVal(void)         //ȷ����ֵ
{
	static INT8U keyV1,keyV2;
    static INT8U KeyState = 0;
    INT8U KeyValue;

	keyV1 = KEYREAD();
    if(keyV1 == 0xff)       //û�м�����
    {
        KeyState = 0;
        return 0;
    }
    
    KeyValue = 0;
    if(KeyState == 0)
    {
        KeyState = 1;
        KeyTimer = 0; 
    }
    else if(KeyTimer >= MS2TENMS(150))     //150ms�ķ�����ʱ�䵽
    {
        KeyState = 0;
        
        keyV2 = KEYREAD();
        if(keyV1==keyV2)
        {
            switch(keyV1)
		    {
            case KEY_UP_MASK:   //0xef:		//��
                KeyValue = 1;
                break;
            case KEY_DOWN_MASK: //0xf7:		//��
                KeyValue = 2;
                break;
            case KEY_LEFT_MASK: //0xfb:		//��
                KeyValue = 3;
                break;
            case KEY_RIGHT_MASK://0xbf:		//��
                KeyValue = 4;
                break;
            case KEY_ENTER_MASK://0xfd:		//ȷ��
                KeyValue = 5;
                break;
            case KEY_CANCEL_MASK://0xdf:	//ȡ��
                KeyValue = 6;
                break;
            case KEY_RESET_MASK://0xf9:		//��λ ȡ�� ȷ�� �� ��ͬʱ����
                KeyValue = 7;
                break; 
            case LCD_RESET_MASK://0xf9:		//��λ ȡ�� ȷ��ͬʱ����
                KeyValue = 8;
                break; 
            case BACK_DOOR0_MASK:			//���Ҽ�ͬʱ��
                KeyValue = 9;
                break;  
            case BACK_DOOR1_MASK:		    //����+ESC
                KeyValue = 10;
                break;
            default:
                break;
            }
        }
    }
    
    return KeyValue;
}


