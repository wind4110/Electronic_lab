#ifndef __LCD12864_H__
#define __LCD12864_H__

#include "msp430g2553.h"
typedef unsigned int uint;
typedef unsigned char uchar;

#define BIT(x)	(1 << (x))

/***************************************
*         Һ������IO�ĺ궨��
***************************************/
#define cyCS      0    //P2.0��Ƭѡ�ź�
#define cySID     1    //P2.1����������  
#define cyCLK     2    //P2.2��ͬ��ʱ��
#define cyPORT    P2OUT  
#define cyDDR     P2DIR

/*******************************************
*��    �ܣ���ʱ N us
********************************************/
void delay_nus(uint n)
{
  uchar i;
  for(i = n;i > 0;i--) 
    _NOP();
} 

/*******************************************
*��    �ܣ���ʱ 1 ms
********************************************/
void delay_1ms(void)
{
  uint i;
  for(i = 1000;i > 0;i--)	  
    _NOP();
}  

/*******************************************
*��    �ܣ���ʱ N ms
********************************************/  
void delay_nms(uint n)
{
  uint i = 0;
  for(i = n;i > 0;i--)
    delay_1ms();
}

/********************************************************************
*��    �ܣ�MCU��Һ��ģ�鷢��1һ���ֽڵ�����
*��    ��:type--��������:0--�������1--��ʾ����,,data--���͵�����
********************************************************************/
void Send(uchar type,uchar data)
{
  uchar firstbyte = 0xf8;
  uchar temp;
  uchar i,j = 3;
  if(type) firstbyte |= 0x02;
  cyPORT |= BIT(cyCS);		 	
  cyPORT &= ~BIT(cyCLK);		
  while(j > 0)
  {
    if(j == 3) 
      temp = firstbyte;
    else if(j == 2) 
      temp = data&0xf0;
    else  
      temp = (data << 4) & 0xf0;
    for(i = 8;i > 0;i--)
    {
      if(temp & 0x80)	
        cyPORT |= BIT(cySID); 
      else			
        cyPORT &= ~BIT(cySID);			
      cyPORT |= BIT(cyCLK);				
      temp <<= 1;
      cyPORT &= ~BIT(cyCLK);
    }
    if(j == 3) 
      delay_nus(600);            //�����ֽ�֮��һ��Ҫ���㹻����ʱ�������׳���ʱ������
    else       
      delay_nus(200);
    j--;
  } 
  cyPORT &= ~BIT(cySID);       
  cyPORT &= ~BIT(cyCS);		
}

/*******************************************
*��    �ܣ����Һ��GDRAM�ڲ����������
********************************************/
void Clear_GDRAM(void)
{
  uchar i,j,k;
  Send(0,0x34);        //����չָ�
  i = 0x80;            
  for(j = 0;j < 32;j++)
  {
    Send(0,i++);  	   
    Send(0,0x80);  
    for(k = 0;k < 16;k++)
    {
      Send(1,0x00); 	
    }
  }
  i = 0x80;
  for(j = 0;j < 32;j++)
  {
    Send(0,i++);	   
    Send(0,0x88); 	   
    for(k = 0;k < 16;k++)
    {
      Send(1,0x00); 
    } 
  }   
  Send(0,0x30);        //�ص�����ָ�	
}

/******************************************************************************
*��    �ܣ���ʾ�ַ���
*��    ����addr--��ʾλ�õ��׵�ַ,pt--ָ����ʾ�ַ������׵�ַ��ָ�룩
*******************************************************************************/
void write_string(uchar addr,const uchar * pt)
{
  Send(0,addr);
  while(* pt)
    Send(1,*(pt++)); 
} 

/******************************************************************************
*��    �ܣ���ʾ���ֳ���
*��    ����addr--��ʾλ�õ��׵�ַ,pt--ָ����ʾ���ݵ�ָ��,num--��ʾ���ֵĸ���
*******************************************************************************/
void write_HZ(uchar addr,const uchar * pt,uchar num)
{
  uchar i;
  Send(0,addr);          
  for(i = 0;i < (num * 2);i++) 
    Send(1,*(pt++)); 
} 

/********************************************
*��    �ܣ���ʾһ����λ���֣�����λС����
*��    ����addr--��ʾ��ַ,����--��ʾ������
********************************************/
void write_SZ(uchar addr,uint shuzi)
{
    uchar temp[5];
    temp[0] = shuzi % 100000 / 10000;
    temp[1] = shuzi % 10000 / 1000;
    temp[2] = shuzi % 1000 / 100;
    temp[3] = shuzi % 100 / 10;
    temp[4] = shuzi % 10;
    //temp[5] = shuzi % 1000 / 100;
    //temp[6] = shuzi % 100 / 10;
    //temp[7] = shuzi % 10;
    
    Send(0,addr);
    Send(1,0x30 + temp[0]);
    Send(1,0x30 + temp[1]);
    Send(1,0x30 + temp[2]);
    Send(1,0x30 + temp[3]);
    Send(1,0x30 + temp[4]);
    //Send(1,0x30 + temp[5]);
    //Send(1,0x30 + temp[6]);
    //Send(1,0x30 + temp[7]);
}


/*******************************************
*��    �ܣ���ʾͼƬ
********************************************/
void write_image(const uchar * pt)
{
  uchar i,j,k;
  Send(0,0x34);        //����չָ�
  i = 0x80;
  for(j = 0;j < 32;j++)			//���ϰ�����ʾ����
  {
    Send(0,i++);				//����ʾY����0x80-0x9f
    Send(0,0x80);				//����ʾX����0x80
    for(k = 0;k < 16;k++)
    {
      Send(1,* pt);
      pt++;
    }
  }
  i = 0x80;
  for(j = 0;j < 32;j++)			//���°�����ʾ����
  {
    Send(0,i++);				//����ʾY����0x80-0x9f
    Send(0,0x88);				//����ʾX����0x88
    for(k = 0;k < 16;k++)
    {
      Send(1,* pt);
      pt++;
    }
  }
  Send(0,0x36);        //����չָ�����ͼ��ʾON
}

void write_dot(char x,char y)
{
	Send(0,0x34);        //����չָ�
	Send(0,0x80+y%32);
	Send(0,0x80+8*(y/32)+x/16);
        if(x%16/8)
        {
          Send(1,0);
          Send(1,128>>(x%8));
        }
        else Send(1,128>>(x%8));
	Send(0,0x36);        //����չָ�����ͼ��ʾON
}


/*******************************************
*��    �ܣ���ʼ��Һ��ģ��
********************************************/
void init_LCD(void)
{
  cyDDR |= BIT(cyCLK) + BIT(cySID) + BIT(cyCS);   //��Ӧ��λ�˿�����Ϊ���
  delay_nms(100);                 //��ʱ�ȴ�Һ����ɸ�λ
  Send(0,0x30);  /*��������:һ����8λ����,����ָ�*/
  delay_nus(50);
  Send(0,0x02);  /*DDRAM��ַ��λ*/
  delay_nus(50);
  Send(0,0x0c);  /*��ʾ�趨:����ʾ,����ʾ���,������ǰ��ʾλ��������*/
  delay_nus(50);
  Send(0,0x01);  /*��������DDRAM��λַ����������Ϊ��00H��*/
  delay_nus(50);
  Send(0,0x06);  /*�������ã����趨:��ʾ�ַ�/����������λ,DDRAM��ַ��1*/
  delay_nus(50);		
}  
#endif



