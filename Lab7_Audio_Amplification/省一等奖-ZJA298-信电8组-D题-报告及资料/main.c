#include "msp430g2553.h"
#include "LCD12864.h"
#define pulseIn BIT4
#define ad9850_w_clk_up   P2OUT|=BIT3;      //P2.0口接ad9850的w_clk脚上升沿
#define ad9850_w_clk_dw   P2OUT&=~BIT3;      //P2.0口接ad9850的w_clk脚下降沿
#define ad9850_fq_up_up   P2OUT|=BIT4;        //P2.1口接ad9850的fq_up脚上升沿
#define ad9850_fq_up_dw  P2OUT&=~BIT4;        //P2.1口接ad9850的fq_up脚下降沿
#define ad9850_rest_up   P2OUT|=BIT5;        //P2.2口接ad9850的rest脚上升沿
#define ad9850_rest_dw    P2OUT&=~BIT5;     //P2.2口接ad9850的rest脚下降沿
#define Key BIT3


//              ad9850复位(串口模式)                 
//---------------------------------------------------//
void ad9850_reset_serial()
{
ad9850_w_clk_dw;
ad9850_fq_up_dw;
//rest信号
ad9850_rest_dw;
ad9850_rest_up;
ad9850_rest_dw;
//w_clk信号
ad9850_w_clk_dw;
ad9850_w_clk_up;
ad9850_w_clk_dw;
//fq_up信号
ad9850_fq_up_dw;
ad9850_fq_up_up;
ad9850_fq_up_dw;
}

void ad9850_wr_serial(unsigned char w0,double frequence)
{
unsigned char i,w;
long int y;
double x;
//计算频率的HEX值
x=4294967295/125;//适合125M晶振
//如果时钟频率不为125MHZ，修改该处的频率值，单位MHz  ！！！
frequence=frequence*x/1000000;
//frequence=frequence*x;
y=(long)frequence;
//写w4数据
w=(y>>=0);
for(i=0;i<8;i++)
{
if((w>>i)&0x01)
P1OUT|=BIT7;
else 
P1OUT&=~BIT7;
ad9850_w_clk_up;
ad9850_w_clk_dw;
}
//写w3数据
w=(y>>8);
for(i=0;i<8;i++)
{
if((w>>i)&0x01)
P1OUT|=BIT7;
else 
P1OUT&=~BIT7;
ad9850_w_clk_up;
ad9850_w_clk_dw;
}
//写w2数据
w=(y>>16);
for(i=0;i<8;i++)
{
if((w>>i)&0x01)
P1OUT|=BIT7;
else 
P1OUT&=~BIT7;
ad9850_w_clk_up;
ad9850_w_clk_dw;
}
//写w1数据
w=(y>>24);
for(i=0;i<8;i++)
{
if((w>>i)&0x01)
P1OUT|=BIT7;
else 
P1OUT&=~BIT7;
ad9850_w_clk_up;
ad9850_w_clk_dw;
}
//写w0数据
w=w0;   
for(i=0;i<8;i++)
{
if((w>>i)&0x01)
P1OUT|=BIT7;
else 
P1OUT&=~BIT7;
ad9850_w_clk_up;
ad9850_w_clk_dw;
}
//移入始能
ad9850_fq_up_up;
ad9850_fq_up_dw;
}

double frq=200000;

unsigned long pulseNum=0;

void main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  
  TA1CTL|=TASSEL_2+TACLR+MC_3+ID_3;//采取内部时钟源1.04M，八分频，增减计数模式
  TA1CCTL0=CCIE;//启用定时器中断
  TA1CCR0=65535;//计数1S
  
  P1DIR&=~pulseIn;         //输入
  P1SEL&=~pulseIn;
  P1REN|= pulseIn;         //加上拉电阻
  P1OUT|= pulseIn;         
  
  P1DIR&=~Key;         //输入
  P1SEL&=~Key;
  P1REN|= Key;         //加上拉电阻
  P1OUT|=Key;          
  
  P1DIR|=BIT0+BIT1+BIT2+BIT7;
  P1OUT&=~(BIT0+BIT1+BIT2+BIT7);
  
  P2SEL&=~(BIT0+BIT1+BIT2);                                                           //P2为I/O模式
  P2DIR|=BIT0+BIT1+BIT2+BIT3+BIT4+BIT5;
  init_LCD();
  
  ad9850_reset_serial();
  ad9850_wr_serial(0x00,frq);

  P1IE|=pulseIn;           //设中断
  P1IES|=pulseIn;   	//上升沿中断
  P1IE|=Key;           //设中断
  P1IES|=Key;          //上升沿中断
  P1IFG=0;              //中断清零
 _BIS_SR(LPM0_bits+GIE);  //开启总中断
 while(1);
 
  }

#pragma vector=PORT1_VECTOR
__interrupt void P1_4(void)
{
 unsigned int i;
  if (!(P1IN&Key))
  {     
    for(i=100;i>0;i--); 
    while(!(P1IN&Key));
    for(i=100;i>0;i--);
    while(!(P1IN&Key));
    if((P1OUT&BIT0)&&(P1OUT&BIT1))
      P1OUT^=BIT2;
    if(P1OUT&BIT0)
      P1OUT^=BIT1;
      P1OUT^=BIT0; 
    P1IFG&=~Key;
  }
  P1IFG&=~Key;
  
 if((P1IFG&pulseIn) == pulseIn)
 {
  P1IFG&=~pulseIn;
  pulseNum++;
 }
}

#pragma vector=TIMER1_A0_VECTOR 
__interrupt void  oneSecond(void)
 { 
     write_SZ(0x83,pulseNum*195/181);
     //frq=frq+10000;
     //if (frq>300000)frq=100000;
     //ad9850_wr_serial(0x00,frq);
     pulseNum=0;
 }