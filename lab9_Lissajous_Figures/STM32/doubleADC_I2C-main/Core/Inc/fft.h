#ifndef _FFT_H_
#define _FFT_H_

#include "arm_math.h"
#include "arm_const_structs.h"

extern const int POINT_COUNT_2048;
extern const int POINT_COUNT_4096;

void FFT(float_t *input,float_t * output,uint32_t num);

// void fft_ex(unsigned short*input,float *output,uint32_t num);
void FFT_Power_Mag(float_t *input, float *mag, uint32_t num);

#endif
