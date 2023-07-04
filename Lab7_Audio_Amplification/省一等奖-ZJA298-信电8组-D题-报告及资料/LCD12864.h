#ifndef __LCD12864_H__
#define __LCD12864_H__

#include "msp430g2553.h"
typedef unsigned int uint;
typedef unsigned char uchar;

#define BIT(x)	(1 << (x))

/***************************************
*         液晶控制IO的宏定义
***************************************/
#define cyCS      0    //P2.0，片选信号
#define cySID     1    //P2.1，串行数据  
#define cyCLK     2    //P2.2，同步时钟
#define cyPORT    P2OUT  
#define cyDDR     P2DIR

/*******************************************
*功    能：延时 N us
********************************************/
void delay_nus(uint n)
{
  uchar i;
  for(i = n;i > 0;i--) 
    _NOP();
} 

/*******************************************
*功    能：延时 1 ms
********************************************/
void delay_1ms(void)
{
  uint i;
  for(i = 1000;i > 0;i--)	  
    _NOP();
}  

/*******************************************
*功    能：延时 N ms
********************************************/  
void delay_nms(uint n)
{
  uint i = 0;
  for(i = n;i > 0;i--)
    delay_1ms();
}

/********************************************************************
*功    能：MCU向液晶模块发送1一个字节的数据
*参    数:type--数据类型:0--控制命令，1--显示数据,,data--发送的数据
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
      delay_nus(600);            //三个字节之间一定要有足够的延时，否则易出现时序问题
    else       
      delay_nus(200);
    j--;
  } 
  cyPORT &= ~BIT(cySID);       
  cyPORT &= ~BIT(cyCS);		
}

/*******************************************
*功    能：清除液晶GDRAM内部的随机数据
********************************************/
void Clear_GDRAM(void)
{
  uchar i,j,k;
  Send(0,0x34);        //打开扩展指令集
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
  Send(0,0x30);        //回到基本指令集	
}

/******************************************************************************
*功    能：显示字符串
*参    数：addr--显示位置的首地址,pt--指向显示字符串的首地址（指针）
*******************************************************************************/
void write_string(uchar addr,const uchar * pt)
{
  Send(0,addr);
  while(* pt)
    Send(1,*(pt++)); 
} 

/******************************************************************************
*功    能：显示汉字程序
*参    数：addr--显示位置的首地址,pt--指向显示数据的指针,num--显示汉字的个数
*******************************************************************************/
void write_HZ(uchar addr,const uchar * pt,uchar num)
{
  uchar i;
  Send(0,addr);          
  for(i = 0;i < (num * 2);i++) 
    Send(1,*(pt++)); 
} 

/********************************************
*功    能：显示一个四位数字（含两位小数）
*参    数：addr--显示地址,数字--显示的数字
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
*功    能：显示图片
********************************************/
void write_image(const uchar * pt)
{
  uchar i,j,k;
  Send(0,0x34);        //打开扩展指令集
  i = 0x80;
  for(j = 0;j < 32;j++)			//送上半屏显示数据
  {
    Send(0,i++);				//送显示Y坐标0x80-0x9f
    Send(0,0x80);				//送显示X坐标0x80
    for(k = 0;k < 16;k++)
    {
      Send(1,* pt);
      pt++;
    }
  }
  i = 0x80;
  for(j = 0;j < 32;j++)			//送下半屏显示数据
  {
    Send(0,i++);				//送显示Y坐标0x80-0x9f
    Send(0,0x88);				//送显示X坐标0x88
    for(k = 0;k < 16;k++)
    {
      Send(1,* pt);
      pt++;
    }
  }
  Send(0,0x36);        //打开扩展指令集，绘图显示ON
}

void write_dot(char x,char y)
{
	Send(0,0x34);        //打开扩展指令集
	Send(0,0x80+y%32);
	Send(0,0x80+8*(y/32)+x/16);
        if(x%16/8)
        {
          Send(1,0);
          Send(1,128>>(x%8));
        }
        else Send(1,128>>(x%8));
	Send(0,0x36);        //打开扩展指令集，绘图显示ON
}


/*******************************************
*功    能：初始化液晶模块
********************************************/
void init_LCD(void)
{
  cyDDR |= BIT(cyCLK) + BIT(cySID) + BIT(cyCS);   //相应的位端口设置为输出
  delay_nms(100);                 //延时等待液晶完成复位
  Send(0,0x30);  /*功能设置:一次送8位数据,基本指令集*/
  delay_nus(50);
  Send(0,0x02);  /*DDRAM地址归位*/
  delay_nus(50);
  Send(0,0x0c);  /*显示设定:开显示,不显示光标,不做当前显示位反白闪动*/
  delay_nus(50);
  Send(0,0x01);  /*清屏，将DDRAM的位址计数器调整为“00H”*/
  delay_nus(50);
  Send(0,0x06);  /*功能设置，点设定:显示字符/光标从左到右移位,DDRAM地址加1*/
  delay_nus(50);		
}  
#endif



