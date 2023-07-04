#ifndef _FFT_H_
#define _FFT_H_

#include "stm32f1xx.h"

#define NPT 1024

#define u32 uint32_t
#define u16 uint16_t

extern u32 lBufInArray[NPT];
extern u32 lBufOutArray[NPT];
extern u32 lBufMagArray[NPT];
extern u32 lBUFPHASE[NPT];  

void InitBufInArray(void);
void InitBufInArray2(void);
void GetPowerMag(void);
void PowerPhase(u16 nfill);
void select_max(u32 *a);
void lcd_print_fft();
void lcd_show_fft(unsigned int *p);


double Hanning_Win(int i ,int N);   //ººÄþ´°
double Hamming_Win(int i ,int N);   //º£Ã÷´°
double Blackman_Win(int i ,int N);  //²¼À³¿ËÂü´°
double Flattop_Win(int i ,int N);   //Æ½¶¥´°

float findmid(float *vol,int n);

#endif
