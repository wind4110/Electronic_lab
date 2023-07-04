#include  "systemInit.h"
#include  "lcd12864.h"

/*   SID== PB4,CLK== PB5   */

#define LCD_SID  GPIO_PIN_4
#define LCD_CLK  GPIO_PIN_5
#define com 0
#define dat 1

unsigned int IMG[512];
/*********************************************************************

���IO�ĳ�ʼ��

**********************************************************************/
void softspi_init(void)
{/*   SID== PB4,CLK== PB5   */
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5);//���������ܽ�Ϊ���ģʽ
GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5,0x0);
}

/*********************************************************************

��12864д��һ���ֽ�

**********************************************************************/

void lcd_byte(unsigned char byte)    //LCD���ֹ�ָ��
{
unsigned char i;
for(i=0;i<8;i++)
{
GPIOPinWrite(GPIO_PORTB_BASE,LCD_CLK,0x0);     //     lcd_clk=0;
if(byte&0x80)
GPIOPinWrite(GPIO_PORTB_BASE,LCD_SID,1<<4);
else                                           //     lcd_rw=CY;
GPIOPinWrite(GPIO_PORTB_BASE,LCD_SID,0x0);
byte<<=1;
GPIOPinWrite(GPIO_PORTB_BASE,LCD_CLK,1<<5);    //     lcd_clk=1;
}
}

/**************lCD���딵���ӳ�ʽ*****************************/
void lcd_write(unsigned char dat_com,unsigned char byt)    //lCD���딵���ӳ�ʽ
{
unsigned char temp;
if(dat_com==0)     //�����0,�t��ʾ����ָ��
temp=0xf8;     //11111,RS(0),RW(0),0
else            //��1,���딵��
temp=0xfa;     //11111,RS(1),RW(0),0


lcd_busy();            //�z�ylCD�Ƿ��æ��B
lcd_byte(temp);        //����λ����lCD
lcd_byte(byt&0xf0); //�������λ
lcd_byte(byt<<4);   //�������λ

}

/**********��lCD���xȡһ������**************************/
unsigned char lcd_read(void)    //��lCD���xȡһ������
{
unsigned char i,temp1,temp2;
temp1 = 0;
temp2 = 0;
GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_4);
for(i=0;i<8;i++)
{
temp1=temp1<<1;
GPIOPinWrite(GPIO_PORTB_BASE,LCD_CLK,0x0);    //lcd_clk = 0;
GPIOPinWrite(GPIO_PORTB_BASE,LCD_CLK,1<<5);   //lcd_clk = 1;
//GPIOPinWrite(GPIO_PORTB_BASE,LCD_CLK,0x0);    //lcd_clk = 0;
if(GPIOPinRead(GPIO_PORTB_BASE,LCD_SID))      //�xȡIO��B
temp1++;
}
for(i=0;i<8;i++)
{
temp2=temp2<<1;
GPIOPinWrite(GPIO_PORTB_BASE,LCD_CLK,0x0);    //lcd_clk = 0;
GPIOPinWrite(GPIO_PORTB_BASE,LCD_CLK,1<<5);   //lcd_clk = 1;
//GPIOPinWrite(GPIO_PORTB_BASE,LCD_CLK,0x0);    //lcd_clk = 0;
if(GPIOPinRead(GPIO_PORTB_BASE,LCD_SID))      //�xȡIO��B
temp2++;
}
GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_4);
temp2=temp2>>4;
return ((0xf0&temp1)+(0x0f&temp2));
}

/*******�ȴ�lCD���e��B**************************/
void lcd_busy(void)    //�ȴ�lCD���e��B
{
do
lcd_byte(0xfc);//11111,RW(1),RS(0),0����ָ��
while(0x80&lcd_read());
}

/*****lCD��ʼ��******************************/
void lcd_init(void)//lCD��ʼ��
{
lcd_write(com,0x30); //����ָ���.
SysCtlDelay(150 * (SysCtlClockGet( ) / 3000)); 
lcd_write(com,0x0c); //���_�@ʾ.�P�]���,�P�]����
SysCtlDelay(150 * (SysCtlClockGet( ) / 3000)); 
lcd_write(com,0x01); //����ָ��.
SysCtlDelay(150 * (SysCtlClockGet( ) / 3000)); 
lcd_write(com,0x06); //AC�ԄӼ�һ
SysCtlDelay(150 * (SysCtlClockGet( ) / 3000)); 
}


/************LCD�@ʾ�h���ӳ�ʽ********************/
void Show(char address,char *hz)
{
 lcd_write(com,address);
 while(*hz != '\0')
 {
  lcd_write(dat,*hz);
  hz++;
 }
}

void LcdDrawClear(void)
   {unsigned char  disp_page,i,j;
    
    lcd_write(com,0x34); //����ָ�
    for(i=0;i<32;i++)
      {disp_page=i|0x80;
       lcd_write(com,disp_page);
       lcd_write(com,0x80);
       for(j=0;j<32;j++)
          {
            lcd_write(dat,0x00);
          }
      }
   }

/********************��㺯��**********************************/
void lcd_dadian(unsigned char X,unsigned char Y)
{
     unsigned char i= 0, j = 0;
     unsigned char temp1 = 0x00,temp2 = 0x00;   

     i = X / 16;
     j = X % 16;      
     Y = 63 - Y;
     if(Y>=0&&Y<=31)
     {            //�����ݲ���
         lcd_write(com,0X80+Y);  //��һ��������Y����
         lcd_write(com,0X80+i);  //�ڶ���������X����

        lcd_read();   //���������ն�ȡһ��
        temp1 = lcd_read(); //���Ĳ�����ȡ���ֽ�
        temp2 = lcd_read(); //���岽����ȡ���ֽ�

          //ͼ��ģʽ��д���ݲ���
       lcd_write(com,0X80+Y);  //��һ��������Y����
       lcd_write(com,0X80+i);  //�ڶ���������X����
       if(j>=0 && j<=7)
      {  
          lcd_write(dat,temp1|(0x80>>j));  //��������д���ֽ�����
          lcd_write(dat,temp2);     //���Ĳ���д���ֽ�����
      }
      else
           if(j>7 && j<=15)
          {   
              j = j - 8;  
             lcd_write(dat,temp1);
             lcd_write(dat,temp2|(0x80>>j));
          }   
    }

    else
    if(Y>=32 && Y<=63)
   {
       Y = Y - 32;
       i = i + 8;   

       lcd_write(com,0X80+Y);
       lcd_write(com,0X80+i);
       lcd_read();
       temp1 = lcd_read();
       temp2 = lcd_read();

       lcd_write(com,0X80+Y);
       lcd_write(com,0X80+i);

      if(j>=0 && j<=7)
     { 
        lcd_write(dat,temp1|(0x80>>j));
        lcd_write(dat,temp2);
     }
     else
         if(j>7 && j<=15)
        {   
            j = j - 8;    
            lcd_write(dat,temp1);
            lcd_write(dat,temp2|(0x80>>j));
        }  
    }  
}

/*****************IMG[]����*****************/
void IMG_Clear()
{
   int i=0; 
   for(i=0;i<512;i++) 
   IMG[i]=0x0000; 
}


/********************��㺯��2**********************************/
void PutDot(unsigned char X,unsigned char Y,unsigned char dot)
{
     unsigned char i= 0, j = 0;
     unsigned char temp1 = 0x00,temp2 = 0x00;   
     unsigned int n,tmp;
	
	 lcd_write(com,0x34);	                 //��չָ��� 
     lcd_write(com,0x36);	                 //��չָ���  
	
     n = Y * 8 + X / 16;
     i = X / 16;
     j = X % 16;      
     Y = 63 - Y;
     if(Y>=0&&Y<=31)
     {            //�����ݲ���
          tmp = IMG[n];
          temp1 = tmp>>8;
          temp2 = tmp&0x00FF;
          //ͼ��ģʽ��д���ݲ���
       lcd_write(com,0X80+Y);  //��һ��������Y����
       lcd_write(com,0X80+i);  //�ڶ���������X����
       if(j>=0 && j<=7)
      {  if(dot==1)
          {
           lcd_write(dat,temp1|(0x80>>j));  //��������д���ֽ�����
           lcd_write(dat,temp2);     //���Ĳ���д���ֽ�����
           tmp=temp1|(0x80>>j);
           tmp=tmp<<8;
           tmp=tmp+temp2;
           IMG[n] = tmp;
          }
         else
          {
           lcd_write(dat,temp1&(~0x80>>j));  //��������д���ֽ�����
           lcd_write(dat,temp2);     //���Ĳ���д���ֽ�����
           tmp=temp1&(~0x80>>j);
           tmp=tmp<<8;
           tmp=tmp+temp2;
           IMG[n] = tmp;
          }
           
      }
      else
           if(j>7 && j<=15)
          {   
              j = j - 8; 
             if(dot==1)
             {
              lcd_write(dat,temp1);
              lcd_write(dat,temp2|(0x80>>j));
              tmp=temp1;
              tmp=tmp<<8;
              tmp=tmp+temp2|(0x80>>j);
              IMG[n] = tmp;
             }
            else
             {
             lcd_write(dat,temp1);
             lcd_write(dat,temp2&(~0x80>>j));
             tmp=temp1;
             tmp=tmp<<8;
             tmp=tmp+temp2&(~0x80>>j);
             IMG[n] = tmp;
             } 
          }   
    }

    else
    if(Y>=32 && Y<=63)
   {
       Y = Y - 32;
       i = i + 8;   

        tmp = IMG[n];
        temp1 = tmp>>8;
        temp2 = tmp&0x00FF;

       lcd_write(com,0X80+Y);
       lcd_write(com,0X80+i);

      if(j>=0 && j<=7)
     { 
        if(dot==1)
          {
           lcd_write(dat,temp1|(0x80>>j));  //��������д���ֽ�����
           lcd_write(dat,temp2);     //���Ĳ���д���ֽ�����
           tmp=temp1|(0x80>>j);
           tmp=tmp<<8;
           tmp=tmp+temp2;
           IMG[n] = tmp;
          }
         else
          {
           lcd_write(dat,temp1&(~0x80>>j));  //��������д���ֽ�����
           lcd_write(dat,temp2);     //���Ĳ���д���ֽ�����
           tmp=temp1&(~0x80>>j);
           tmp=tmp<<8;
           tmp=tmp+temp2;
           IMG[n] = tmp;
          }
     }
     else
         if(j>7 && j<=15)
        {   
               j = j - 8; 
             if(dot==1)
             {
              lcd_write(dat,temp1);
              lcd_write(dat,temp2|(0x80>>j));
              tmp=temp1;
              tmp=tmp<<8;
              tmp=tmp+temp2|(0x80>>j);
              IMG[n] = tmp;
             }
            else
             {
             lcd_write(dat,temp1);
             lcd_write(dat,temp2&(~0x80>>j));
             tmp=temp1;
             tmp=tmp<<8;
             tmp=tmp+temp2&(~0x80>>j);
             IMG[n] = tmp;
             } 
        }  
    }  
    lcd_write(com,0x30); 
}

/*****************��ˮƽ�ߺ���*****************************/
void v_Lcd12864DrawLineX_f(unsigned char X0, unsigned char X1, unsigned char Y)
    {    unsigned char Temp ;
        if( X0 > X1 )
          {
           Temp = X1 ;
           X1 = X0 ;
           X0 = Temp ;
          }
        for( ; X0 <= X1 ; X0++ )
        PutDot(X0,Y,1) ;    
    }
/******************����ֱ�ߺ���***************************************/
void v_Lcd12864DrawLineY_f( unsigned char X, unsigned char Y0, unsigned char Y1 )
  {
    unsigned char Temp ;
    if( Y0 > Y1 )
     {
        Temp = Y1 ;
        Y1 = Y0 ;
        Y0 = Temp ;
     }
    for(; Y0 <= Y1 ; Y0++)
    PutDot(X,Y0,1);
  }


/**************************������ֱ�ߺ���***************************/
void lcd_huaxian(unsigned char start_x,unsigned char start_y,unsigned char end_x,unsigned char end_y)
   {
      int t,distance;
      int x = 0 , y = 0 , delta_x, delta_y ;
      int incx, incy ;
 
      delta_x = end_x - start_x;
      delta_y = end_y - start_y;

      if( delta_x > 0 )
        {
          incx = 1;
        }
      else if(delta_x==0)
        {
           v_Lcd12864DrawLineY_f( start_x, start_y, end_y) ;
           return ;
        }

        else
         {
           incx=-1 ;
         }
        if(delta_y>0)
          {
            incy = 1 ;
          }
        else if(delta_y == 0 )
          {
           v_Lcd12864DrawLineX_f( start_x, end_x, start_y);    
           return ;
          }
        else
          {
            incy = -1 ;
          }

         delta_x = (int)fabs(delta_x);    
         delta_y = (int)fabs(delta_y);
          if(delta_x > delta_y)
           {
             distance = delta_x ;
           }
          else
           {
             distance = delta_y ; 
           }

           PutDot(start_x,start_y,1);
           for( t = 0 ; t <= distance+1 ; t++ )
              {
                 PutDot( start_x, start_y,1) ;
                 x += delta_x ;
                 y += delta_y ;
                 if( x > distance )
                  {
                    x -= distance ;
                    start_x += incx ;
                  }
                 if( y > distance )
                  {
                     y -= distance ;
                     start_y += incy ;
                  }
               }
}

/****************��ͼ����*********************************/
void DrawPicture(const unsigned char *pic) //ȫ����ʾͼƬ 
{  
 
unsigned char i,j;  
lcd_write(com,0x34);	                 //��չָ��� 
lcd_write(com,0x36);	                 //��չָ���  

for(i=0;i<32;i++)        //�ϰ�����ʾ  
{  
lcd_write(com,0x80|i);	 //��λ��  
lcd_write(com,0x80);           //��λ��  
for(j=0;j<16;j++)        //256/8=32 byte  
{                        //��λ��ÿ���Զ�����  
  lcd_write(dat,*pic);	 
  pic++;  
}  
}  

for(i=0;i<32;i++)          //�°�����ʾ  
{  
lcd_write(com,0x80|i);     //��λ��  
lcd_write(com,0x88);       //��λ��  
for(j=0;j<16;j++)          //256/8=32 byte  
{  
  lcd_write(dat,*pic);	  
  pic++;  
}  
} 
lcd_write(com,0x30); 
}