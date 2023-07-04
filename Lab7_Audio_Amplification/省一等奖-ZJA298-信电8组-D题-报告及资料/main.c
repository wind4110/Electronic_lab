#include "msp430g2553.h"
#include "LCD12864.h"
#define pulseIn BIT4
#define ad9850_w_clk_up   P2OUT|=BIT3;      //P2.0�ڽ�ad9850��w_clk��������
#define ad9850_w_clk_dw   P2OUT&=~BIT3;      //P2.0�ڽ�ad9850��w_clk���½���
#define ad9850_fq_up_up   P2OUT|=BIT4;        //P2.1�ڽ�ad9850��fq_up��������
#define ad9850_fq_up_dw  P2OUT&=~BIT4;        //P2.1�ڽ�ad9850��fq_up���½���
#define ad9850_rest_up   P2OUT|=BIT5;        //P2.2�ڽ�ad9850��rest��������
#define ad9850_rest_dw    P2OUT&=~BIT5;     //P2.2�ڽ�ad9850��rest���½���
#define Key BIT3


//              ad9850��λ(����ģʽ)                 
//---------------------------------------------------//
void ad9850_reset_serial()
{
ad9850_w_clk_dw;
ad9850_fq_up_dw;
//rest�ź�
ad9850_rest_dw;
ad9850_rest_up;
ad9850_rest_dw;
//w_clk�ź�
ad9850_w_clk_dw;
ad9850_w_clk_up;
ad9850_w_clk_dw;
//fq_up�ź�
ad9850_fq_up_dw;
ad9850_fq_up_up;
ad9850_fq_up_dw;
}

void ad9850_wr_serial(unsigned char w0,double frequence)
{
unsigned char i,w;
long int y;
double x;
//����Ƶ�ʵ�HEXֵ
x=4294967295/125;//�ʺ�125M����
//���ʱ��Ƶ�ʲ�Ϊ125MHZ���޸ĸô���Ƶ��ֵ����λMHz  ������
frequence=frequence*x/1000000;
//frequence=frequence*x;
y=(long)frequence;
//дw4����
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
//дw3����
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
//дw2����
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
//дw1����
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
//дw0����
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
//����ʼ��
ad9850_fq_up_up;
ad9850_fq_up_dw;
}

double frq=200000;

unsigned long pulseNum=0;

void main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  
  TA1CTL|=TASSEL_2+TACLR+MC_3+ID_3;//��ȡ�ڲ�ʱ��Դ1.04M���˷�Ƶ����������ģʽ
  TA1CCTL0=CCIE;//���ö�ʱ���ж�
  TA1CCR0=65535;//����1S
  
  P1DIR&=~pulseIn;         //����
  P1SEL&=~pulseIn;
  P1REN|= pulseIn;         //����������
  P1OUT|= pulseIn;         
  
  P1DIR&=~Key;         //����
  P1SEL&=~Key;
  P1REN|= Key;         //����������
  P1OUT|=Key;          
  
  P1DIR|=BIT0+BIT1+BIT2+BIT7;
  P1OUT&=~(BIT0+BIT1+BIT2+BIT7);
  
  P2SEL&=~(BIT0+BIT1+BIT2);                                                           //P2ΪI/Oģʽ
  P2DIR|=BIT0+BIT1+BIT2+BIT3+BIT4+BIT5;
  init_LCD();
  
  ad9850_reset_serial();
  ad9850_wr_serial(0x00,frq);

  P1IE|=pulseIn;           //���ж�
  P1IES|=pulseIn;   	//�������ж�
  P1IE|=Key;           //���ж�
  P1IES|=Key;          //�������ж�
  P1IFG=0;              //�ж�����
 _BIS_SR(LPM0_bits+GIE);  //�������ж�
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