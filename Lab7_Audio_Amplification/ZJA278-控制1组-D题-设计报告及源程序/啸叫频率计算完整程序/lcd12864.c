#include  "systemInit.h"
#include  "lcd12864.h"

/*   SID== PB4,CLK== PB5   */

#define LCD_SID  GPIO_PIN_4
#define LCD_CLK  GPIO_PIN_5
#define com 0
#define dat 1

unsigned int IMG[512];
/*********************************************************************

相关IO的初始化

**********************************************************************/
void softspi_init(void)
{/*   SID== PB4,CLK== PB5   */
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5);//设置两个管脚为输出模式
GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5,0x0);
}

/*********************************************************************

向12864写入一个字节

**********************************************************************/

void lcd_byte(unsigned char byte)    //LCD寫字節指令
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

/**************lCD寫入數據子程式*****************************/
void lcd_write(unsigned char dat_com,unsigned char byt)    //lCD寫入數據子程式
{
unsigned char temp;
if(dat_com==0)     //如果為0,則表示寫入指令
temp=0xf8;     //11111,RS(0),RW(0),0
else            //為1,寫入數據
temp=0xfa;     //11111,RS(1),RW(0),0


lcd_busy();            //檢測lCD是否為忙狀態
lcd_byte(temp);        //將首位寫入lCD
lcd_byte(byt&0xf0); //寫入高四位
lcd_byte(byt<<4);   //寫入低四位

}

/**********從lCD中讀取一個數據**************************/
unsigned char lcd_read(void)    //從lCD中讀取一個數據
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
if(GPIOPinRead(GPIO_PORTB_BASE,LCD_SID))      //讀取IO狀態
temp1++;
}
for(i=0;i<8;i++)
{
temp2=temp2<<1;
GPIOPinWrite(GPIO_PORTB_BASE,LCD_CLK,0x0);    //lcd_clk = 0;
GPIOPinWrite(GPIO_PORTB_BASE,LCD_CLK,1<<5);   //lcd_clk = 1;
//GPIOPinWrite(GPIO_PORTB_BASE,LCD_CLK,0x0);    //lcd_clk = 0;
if(GPIOPinRead(GPIO_PORTB_BASE,LCD_SID))      //讀取IO狀態
temp2++;
}
GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_4);
temp2=temp2>>4;
return ((0xf0&temp1)+(0x0f&temp2));
}

/*******等待lCD空閑狀態**************************/
void lcd_busy(void)    //等待lCD空閑狀態
{
do
lcd_byte(0xfc);//11111,RW(1),RS(0),0，读指令
while(0x80&lcd_read());
}

/*****lCD初始化******************************/
void lcd_init(void)//lCD初始化
{
lcd_write(com,0x30); //基本指令功能.
SysCtlDelay(150 * (SysCtlClockGet( ) / 3000)); 
lcd_write(com,0x0c); //打開顯示.關閉光標,關閉反白
SysCtlDelay(150 * (SysCtlClockGet( ) / 3000)); 
lcd_write(com,0x01); //清屏指令.
SysCtlDelay(150 * (SysCtlClockGet( ) / 3000)); 
lcd_write(com,0x06); //AC自動加一
SysCtlDelay(150 * (SysCtlClockGet( ) / 3000)); 
}


/************LCD顯示漢字子程式********************/
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
    
    lcd_write(com,0x34); //扩充指令集
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

/********************打点函数**********************************/
void lcd_dadian(unsigned char X,unsigned char Y)
{
     unsigned char i= 0, j = 0;
     unsigned char temp1 = 0x00,temp2 = 0x00;   

     i = X / 16;
     j = X % 16;      
     Y = 63 - Y;
     if(Y>=0&&Y<=31)
     {            //读数据操作
         lcd_write(com,0X80+Y);  //第一步：设置Y坐标
         lcd_write(com,0X80+i);  //第二步：设置X坐标

        lcd_read();   //第三步：空读取一次
        temp1 = lcd_read(); //第四步：读取高字节
        temp2 = lcd_read(); //第五步：读取低字节

          //图形模式的写数据操作
       lcd_write(com,0X80+Y);  //第一步：设置Y坐标
       lcd_write(com,0X80+i);  //第二步：设置X坐标
       if(j>=0 && j<=7)
      {  
          lcd_write(dat,temp1|(0x80>>j));  //第三步：写高字节数据
          lcd_write(dat,temp2);     //第四步：写低字节数据
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

/*****************IMG[]清零*****************/
void IMG_Clear()
{
   int i=0; 
   for(i=0;i<512;i++) 
   IMG[i]=0x0000; 
}


/********************打点函数2**********************************/
void PutDot(unsigned char X,unsigned char Y,unsigned char dot)
{
     unsigned char i= 0, j = 0;
     unsigned char temp1 = 0x00,temp2 = 0x00;   
     unsigned int n,tmp;
	
	 lcd_write(com,0x34);	                 //扩展指令动作 
     lcd_write(com,0x36);	                 //扩展指令动作  
	
     n = Y * 8 + X / 16;
     i = X / 16;
     j = X % 16;      
     Y = 63 - Y;
     if(Y>=0&&Y<=31)
     {            //读数据操作
          tmp = IMG[n];
          temp1 = tmp>>8;
          temp2 = tmp&0x00FF;
          //图形模式的写数据操作
       lcd_write(com,0X80+Y);  //第一步：设置Y坐标
       lcd_write(com,0X80+i);  //第二步：设置X坐标
       if(j>=0 && j<=7)
      {  if(dot==1)
          {
           lcd_write(dat,temp1|(0x80>>j));  //第三步：写高字节数据
           lcd_write(dat,temp2);     //第四步：写低字节数据
           tmp=temp1|(0x80>>j);
           tmp=tmp<<8;
           tmp=tmp+temp2;
           IMG[n] = tmp;
          }
         else
          {
           lcd_write(dat,temp1&(~0x80>>j));  //第三步：写高字节数据
           lcd_write(dat,temp2);     //第四步：写低字节数据
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
           lcd_write(dat,temp1|(0x80>>j));  //第三步：写高字节数据
           lcd_write(dat,temp2);     //第四步：写低字节数据
           tmp=temp1|(0x80>>j);
           tmp=tmp<<8;
           tmp=tmp+temp2;
           IMG[n] = tmp;
          }
         else
          {
           lcd_write(dat,temp1&(~0x80>>j));  //第三步：写高字节数据
           lcd_write(dat,temp2);     //第四步：写低字节数据
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

/*****************画水平线函数*****************************/
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
/******************画垂直线函数***************************************/
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


/**************************画任意直线函数***************************/
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

/****************画图函数*********************************/
void DrawPicture(const unsigned char *pic) //全屏显示图片 
{  
 
unsigned char i,j;  
lcd_write(com,0x34);	                 //扩展指令动作 
lcd_write(com,0x36);	                 //扩展指令动作  

for(i=0;i<32;i++)        //上半屏显示  
{  
lcd_write(com,0x80|i);	 //列位置  
lcd_write(com,0x80);           //行位置  
for(j=0;j<16;j++)        //256/8=32 byte  
{                        //列位置每行自动增加  
  lcd_write(dat,*pic);	 
  pic++;  
}  
}  

for(i=0;i<32;i++)          //下半屏显示  
{  
lcd_write(com,0x80|i);     //列位置  
lcd_write(com,0x88);       //行位置  
for(j=0;j<16;j++)          //256/8=32 byte  
{  
  lcd_write(dat,*pic);	  
  pic++;  
}  
} 
lcd_write(com,0x30); 
}