#include "./fft/fft.h"
#include "math.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./led/bsp_led.h"

u32 lBufInArray[NPT];
u32 lBufOutArray[NPT];
u32 lBufMagArray[NPT];
u32 lBUFPHASE[NPT];
float PI2=6.28318530717959;
float PI=3.14159265358979;
u32 Fs=86000;    
/******************************************************************
函数名称:InitBufInArray()
函数功能:模拟采样数据，采样数据中包含3种频率正弦波
参数说明:
备    注:在lBufInArray数组中，每个数据的高16位存储采样数据的实部，
          低16位存储采样数据的虚部(总是为0)
*******************************************************************/
void InitBufInArray(void)
{
    unsigned short i;
    float fx;                                       //Fn=i*Fs/NPT		//由于此处i是从0开始的，所以不需要再减1
    for(i=0; i<NPT; i++)                            //频率分辨率Fs/1024=187.5Hz,所以每一个i就代表187.5Hz
    {                                               //因为采样频率=Fs，所以屏幕上的一个格子的长度代表(1/Fs)秒
        fx = 1000 * sin(PI2 * i * 1000 / Fs);    //2pi*f*t=2pi*i*f1/Fs,那么f=f1
        lBufInArray[i] = ((signed short)fx) << 16;
    }
} 
 
void InitBufInArray2(void)
{
    unsigned short i;
    float fx;
    for(i=0; i<NPT; i++)
    {
        fx = 3200 * sin(PI2 * i * 1000/ Fs) + 
             1000 * sin(PI2 * i * 2000 / Fs) +
             500 * sin(PI2 * i * 3000 / Fs)+1000;  
        fx *= Hanning_Win(i,NPT); 
        lBufInArray[i] = ((signed short)fx) << 16;
    }
}

void InitBufInArray3(void)
{
    unsigned short i;
    float fx;
    for(i=0; i<NPT; i++)
    {
        fx = 4000 * sin(PI2 * i * 200 / Fs) +
             1000 * sin(PI2 * i * 400 / Fs) +
             500 * sin(PI2 * i * 600/ Fs);   //加了1000只是直流分量加了1000，其他都不变
        lBufInArray[i] = ((signed short)fx) << 16;
    }
}
/******************************************************************
函数名称:GetPowerMag()
函数功能:计算各次谐波幅值                          short 的范围，是-32767 到 32767 。也就是 -(2^15 - 1)到(2^15 - 1)。
参数说明:
备　　注:先将lBufOutArray分解成实部(X)和虚部(Y)，然后计算幅值(sqrt(X*X+Y*Y)
*******************************************************************/
void GetPowerMag(void)
{
    signed short lX,lY;                                                  //算频率的话Fn=i*Fs/NPT		//由于此处i是从0开始的，所以不需要再减1
    float X,Y,Mag;                                                      
    unsigned short i;
    for(i=0; i<NPT/2; i++)                                                  //经过FFT后，每个频率点处的真实幅值  A0=lBufOutArray[0]/NPT
    {                                                                       //                                 Ai=lBufOutArray[i]*2/NPT
        lX  = (lBufOutArray[i] << 16) >> 16;  //lX  = lBufOutArray[i];
        lY  = (lBufOutArray[i] >> 16);
			                                 
        X = NPT * ((float)lX) / 32768;//除以32768再乘65536是为了符合浮点数计算规律,不管他
        Y = NPT * ((float)lY) / 32768;
        Mag = sqrt(X * X + Y * Y) / NPT;
        
        Mag *= 2;/*乘以窗的恢复系数*/
        
        if(i == 0)
            lBufMagArray[i] = (unsigned long)(Mag * 32768);   //0Hz是直流分量，直流分量不需要乘以2
        else
            lBufMagArray[i] = (unsigned long)(Mag * 65536);
    }
}
 
 
void PowerPhase(u16 nfill)
 
{
    unsigned short i;
    signed short lX,lY;  
 
		for (i=0; i < NPT/2; i++)
		{
						lX= (lBufOutArray[i]<<16)>>16; /* 取低16bit，sine_cosine --> cos */
						lY= (lBufOutArray[i] >> 16);   /* 取高16bit，sine_cosine --> sin */    
						{
								float X=  NPT*((float)lX)/32768;
								float Y = NPT*((float)lY)/32768;
								float phase = atan(Y/X);
								 if (Y>=0)
								 {
										if (X>=0)
											;
										else
										 phase+=PI;  
								 }
								 else
								 {             
										if (X>=0)
											phase+=PI2;                  
										else 
											phase+=PI;                    
								 }                            
								lBUFPHASE[i] = phase*180.0/PI;
						}    
				}
}
 
void lcd_show_fft(unsigned int *p)
{
	for(int i = 0;i < NPT/2; i++)
    {
        ILI9341_DrawLine( 10+i, 240, 10+i, 240-p[i]*240/5000 );
    }
    
	
}
/***********************************************
找最大值，次大值……对应的频率，分析波形
*************************************************/
void select_max(u32 *a)
{
	  int i,j;
	  float k,k1,m;
      float aMax =0.0,aSecondMax = 0.0,aThirdMax = 0.0,aFourthMax=0.0,aFifthMax = 0.0;
      float fMax =0.0,fSecondMax = 0.0,fThirdMax = 0.0,fFourthMax=0.0,fFifthMax = 0.0;
	  int nMax = 0,nSecondMax = 0,nThirdMax = 0,nFourthMax = 0,nFifthMax = 0;
    
	  for ( i = 1; i < NPT/2; i++)//i必须是1，是0的话，会把直流分量加进去！！！！
      { 
        if (a[i]>aMax)
        {
            aMax = a[i]; 
            nMax=i;
			fMax=(float)nMax*Fs/NPT;
        }
      }
	  for ( i=1; i < NPT/2; i++)
      {
		if (nMax == i)
		{
		    continue;//跳过原来最大值的下标，直接开始i+1的循环
		}
        if (a[i]>aSecondMax&&a[i]>a[i+1]&&a[i]>a[i-1])
        {
            aSecondMax = a[i]; 
			nSecondMax=i;
			fSecondMax=(float)nSecondMax*Fs/NPT;
        }
      }
	  for ( i=1; i < NPT/2; i++)
      {
		if (nMax == i||nSecondMax==i)
		{
			continue;//跳过原来最大值的下标，直接开始i+1的循环
		}
        if (a[i]>aThirdMax&&a[i]>a[i+1]&&a[i]>a[i-1])
        {
            aThirdMax = a[i]; 
			nThirdMax=i;
			fThirdMax=(float)nThirdMax*Fs/NPT;
        }
      }
	  for ( i=1; i < NPT/2; i++)
      {
		if (nMax == i||nSecondMax==i||nThirdMax==i)
		{
		continue;//跳过原来最大值的下标，直接开始i+1的循环
	    }
        if (a[i]>aFourthMax&&a[i]>a[i+1]&&a[i]>a[i-1])
        {
            aFourthMax = a[i]; 
		    nFourthMax=i;
			fFourthMax=(float)nFourthMax*Fs/NPT;
        }
    }
      for ( i=1; i < NPT/2; i++)
      {
		if (nMax == i||nSecondMax==i||nThirdMax==i||nFourthMax ==i )
		{
		    continue;//跳过原来最大值的下标，直接开始i+1的循环
	    }
        if (a[i]>aFifthMax&&a[i]>a[i+1]&&a[i]>a[i-1])
        {
            aFifthMax = a[i]; 
		    nFifthMax=i;
			fFifthMax=(float)nFifthMax*Fs/NPT;
        }
      }
      
    
    char LCDtemp[100];
      ILI9341_DispStringLine_EN( LINE(1), "F  :   V" );
      sprintf(LCDtemp,"%d : %.2f,%.2f", nMax,fMax,aMax);
    ILI9341_DispStringLine_EN( LINE(2), LCDtemp );
      sprintf(LCDtemp,"%d : %.2f,%.2f", nSecondMax,fSecondMax,aSecondMax);
    ILI9341_DispStringLine_EN( LINE(3), LCDtemp );
      sprintf(LCDtemp,"%d : %.2f,%.2f", nThirdMax,fThirdMax,aThirdMax);
    ILI9341_DispStringLine_EN( LINE(4), LCDtemp );
      sprintf(LCDtemp,"%d : %.2f,%.2f", nFourthMax,fFourthMax,aFourthMax);
    ILI9341_DispStringLine_EN( LINE(5), LCDtemp );
      sprintf(LCDtemp,"%d : %.2f,%.2f", nFifthMax,fFifthMax,aFifthMax);
    ILI9341_DispStringLine_EN( LINE(6), LCDtemp );
      
    float thd = sqrt(aSecondMax*aSecondMax+aThirdMax*aThirdMax+aFourthMax*aFourthMax+aFifthMax*aFifthMax) / aMax * 100;
    sprintf(LCDtemp,"THD = %.2f%%", thd);
    ILI9341_DispStringLine_EN( LINE(7), LCDtemp );
    
   
}

void lcd_print_fft()
{
    unsigned int i = 0,j = 0;
	for(i=0;i<NPT/2;i++)
    {
	  if(lBufMagArray[i]<=50)//看情况调，若是数字太跳就调,把小的幅值过滤，以防干扰
		{
			lBufMagArray[i] = 0;
		}
    }
	select_max(lBufMagArray);
}
 

/*******************************窗函数*******************************/
//i的范围为[0,N-1],包括0和N—1
//i为当前点位置，N为fft点数
double Hanning_Win(int i ,int N)   //汉宁窗
{
     return (0.5-0.5*cos((2 * PI * i )/(N)));
}

double Hamming_Win(int i ,int N)   //海明窗
{
    return (0.54-0.46*cos((2 * PI * i )/(N - 1)));
}

double Blackman_Win(int i ,int N)  //布莱克曼窗
{
    double cos_factor=(2 * PI * i )/(N - 1);
    return ( 0.50-0.50*cos(cos_factor)+0.08*cos(2*cos_factor) );
}

double Flattop_Win(int i ,int N)   //平顶窗
{
 double cos_factor=(2 * PI * i )/(N - 1);
 return ( 1-1.93*cos(cos_factor)+1.29*cos(2*cos_factor)-0.388*cos(3*cos_factor)+0.0322*cos(4*cos_factor) )/4.634;
}

void FFT_withWin(u32 *lBufInArray,double (*Win)(int,int))
{
    for(int i = 0;i < NPT;i++)
    {
        lBufInArray[i] *= Win(i,NPT);
    }
}

/*************************失真度**********************************/

float findmid(float *vol,int n)
{
    float max = 0.0,min = 5.0;
    for(int i = 0; i < n;i++)
    {
        if(vol[i]>max)
            max = vol[i];
        if(vol[i]<min)
            min = vol[i];
    }
    return max;
}

