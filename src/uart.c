

#include "system.h"
#include "uart.h"
#include "sc16is752.h"


COM_BUF Com_Buf[5];

/*******************************************************************
������������λ�����,����ʵ�ʴ���������������ݸ���
*******************************************************************/
int PutComBuf(COM_BUF *pbuf, unsigned char *pdata, int datasize)
{
    int i, putlen;
    
    putlen = 0;
    for(i=0; i<datasize; ++i)
    {
        if((pbuf->tail == 0) && (pbuf->head == RINGBUF_SIZE-1))
            break;		//buf is full
        if(pbuf->tail-1 == pbuf->head)
            break;		//buf is full
        
        pbuf->buf[pbuf->head] = pdata[i];
        if(++(pbuf->head) >= RINGBUF_SIZE)
            pbuf->head = 0;
        
        ++putlen;
    }
    
    return putlen;
}

/*******************************************************************
�ӻ��λ�����ȡ����,����ʵ�ʴ���������������ݸ���
*******************************************************************/
int GetComBuf(COM_BUF *pbuf, unsigned char *pdata, int datasize)
{
    int i, getlen;
    
    getlen = 0;
    for(i=0; i<datasize; ++i)
    {
        if(pbuf->tail == pbuf->head)
            break;		//buf is empty
        
        pdata[i] = pbuf->buf[pbuf->tail];
        if(++(pbuf->tail) >= RINGBUF_SIZE)
            pbuf->tail = 0;
        
        ++getlen;
    }
    
    return getlen;
}

/*******************************************************************
������ݻ�����������
*******************************************************************/
void ClearCombuf(COM_BUF *pbuf)
{
    pbuf->head = 0;
    pbuf->tail = 0;
}


void Msp430_UartSend(unsigned char *src,int len,int com);
/*******************************************************************
�Ӵ��ڷ�������
*******************************************************************/
void Uart_SendData(unsigned char *src,int len,int com)
{
    switch(com)
    {
    //case GPRS:
    case COM1:
        Msp430_UartSend(src,len,com);
        break;
    case COM2:
    case COM3:
    case COM4:
    case COM5:
        IS752_SendData(src,len,com);
        break;
    default:
        break;
    }
}

/*******************************************************************
�Ӵ��ڽ�������
*******************************************************************/
int Uart_RcvData(unsigned char *dst,int len,int com)
{
    COM_BUF *pbuf;
    pbuf = &Com_Buf[com];
    return GetComBuf(pbuf,dst,len);
}

/*******************************************************************
������ڽ��ջ���
*******************************************************************/
void Uart_ClearRcvBuf(int com)
{
    ClearCombuf(&Com_Buf[com]);  
}


/*******************************************************************
430�Դ����ڵķ��ͺ���
*******************************************************************/
void Msp430_UartSend(unsigned char *src,int len,int com)
{
    int i;
    unsigned char *psrc = (unsigned char *)src;
    
    for(i=0;i<len;i++)
    {
        while(!(IFG2&UCA0TXIFG));
        UCA0TXBUF =*psrc++;
    }
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIA0RX_ISR(void)
{
    INT8U temp;
    temp=UCA0RXBUF;
    PutComBuf(&Com_Buf[COM1],&temp,1);
}

