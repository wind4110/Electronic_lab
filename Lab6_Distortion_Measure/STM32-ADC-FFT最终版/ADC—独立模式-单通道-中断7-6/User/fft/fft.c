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
��������:InitBufInArray()
��������:ģ��������ݣ����������а���3��Ƶ�����Ҳ�
����˵��:
��    ע:��lBufInArray�����У�ÿ�����ݵĸ�16λ�洢�������ݵ�ʵ����
          ��16λ�洢�������ݵ��鲿(����Ϊ0)
*******************************************************************/
void InitBufInArray(void)
{
    unsigned short i;
    float fx;                                       //Fn=i*Fs/NPT		//���ڴ˴�i�Ǵ�0��ʼ�ģ����Բ���Ҫ�ټ�1
    for(i=0; i<NPT; i++)                            //Ƶ�ʷֱ���Fs/1024=187.5Hz,����ÿһ��i�ʹ���187.5Hz
    {                                               //��Ϊ����Ƶ��=Fs��������Ļ�ϵ�һ�����ӵĳ��ȴ���(1/Fs)��
        fx = 1000 * sin(PI2 * i * 1000 / Fs);    //2pi*f*t=2pi*i*f1/Fs,��ôf=f1
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
             500 * sin(PI2 * i * 600/ Fs);   //����1000ֻ��ֱ����������1000������������
        lBufInArray[i] = ((signed short)fx) << 16;
    }
}
/******************************************************************
��������:GetPowerMag()
��������:�������г����ֵ                          short �ķ�Χ����-32767 �� 32767 ��Ҳ���� -(2^15 - 1)��(2^15 - 1)��
����˵��:
������ע:�Ƚ�lBufOutArray�ֽ��ʵ��(X)���鲿(Y)��Ȼ������ֵ(sqrt(X*X+Y*Y)
*******************************************************************/
void GetPowerMag(void)
{
    signed short lX,lY;                                                  //��Ƶ�ʵĻ�Fn=i*Fs/NPT		//���ڴ˴�i�Ǵ�0��ʼ�ģ����Բ���Ҫ�ټ�1
    float X,Y,Mag;                                                      
    unsigned short i;
    for(i=0; i<NPT/2; i++)                                                  //����FFT��ÿ��Ƶ�ʵ㴦����ʵ��ֵ  A0=lBufOutArray[0]/NPT
    {                                                                       //                                 Ai=lBufOutArray[i]*2/NPT
        lX  = (lBufOutArray[i] << 16) >> 16;  //lX  = lBufOutArray[i];
        lY  = (lBufOutArray[i] >> 16);
			                                 
        X = NPT * ((float)lX) / 32768;//����32768�ٳ�65536��Ϊ�˷��ϸ������������,������
        Y = NPT * ((float)lY) / 32768;
        Mag = sqrt(X * X + Y * Y) / NPT;
        
        Mag *= 2;/*���Դ��Ļָ�ϵ��*/
        
        if(i == 0)
            lBufMagArray[i] = (unsigned long)(Mag * 32768);   //0Hz��ֱ��������ֱ����������Ҫ����2
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
						lX= (lBufOutArray[i]<<16)>>16; /* ȡ��16bit��sine_cosine --> cos */
						lY= (lBufOutArray[i] >> 16);   /* ȡ��16bit��sine_cosine --> sin */    
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
�����ֵ���δ�ֵ������Ӧ��Ƶ�ʣ���������
*************************************************/
void select_max(u32 *a)
{
	  int i,j;
	  float k,k1,m;
      float aMax =0.0,aSecondMax = 0.0,aThirdMax = 0.0,aFourthMax=0.0,aFifthMax = 0.0;
      float fMax =0.0,fSecondMax = 0.0,fThirdMax = 0.0,fFourthMax=0.0,fFifthMax = 0.0;
	  int nMax = 0,nSecondMax = 0,nThirdMax = 0,nFourthMax = 0,nFifthMax = 0;
    
	  for ( i = 1; i < NPT/2; i++)//i������1����0�Ļ������ֱ�������ӽ�ȥ��������
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
		    continue;//����ԭ�����ֵ���±ֱ꣬�ӿ�ʼi+1��ѭ��
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
			continue;//����ԭ�����ֵ���±ֱ꣬�ӿ�ʼi+1��ѭ��
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
		continue;//����ԭ�����ֵ���±ֱ꣬�ӿ�ʼi+1��ѭ��
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
		    continue;//����ԭ�����ֵ���±ֱ꣬�ӿ�ʼi+1��ѭ��
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
	  if(lBufMagArray[i]<=50)//�����������������̫���͵�,��С�ķ�ֵ���ˣ��Է�����
		{
			lBufMagArray[i] = 0;
		}
    }
	select_max(lBufMagArray);
}
 

/*******************************������*******************************/
//i�ķ�ΧΪ[0,N-1],����0��N��1
//iΪ��ǰ��λ�ã�NΪfft����
double Hanning_Win(int i ,int N)   //������
{
     return (0.5-0.5*cos((2 * PI * i )/(N)));
}

double Hamming_Win(int i ,int N)   //������
{
    return (0.54-0.46*cos((2 * PI * i )/(N - 1)));
}

double Blackman_Win(int i ,int N)  //����������
{
    double cos_factor=(2 * PI * i )/(N - 1);
    return ( 0.50-0.50*cos(cos_factor)+0.08*cos(2*cos_factor) );
}

double Flattop_Win(int i ,int N)   //ƽ����
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

/*************************ʧ���**********************************/

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

