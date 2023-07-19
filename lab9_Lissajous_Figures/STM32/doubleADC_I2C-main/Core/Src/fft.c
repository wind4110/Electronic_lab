/**
  ******************************************************************************
  * @file    fft.c
  * @author  Zixuan Song
  * @brief   This file provides code of algorithms 
  *          for computing the FFT of real data sequences. 
  * @ref     https://www.keil.com/pack/doc/CMSIS/DSP/html/group__RealFFT.html
  ******************************************************************************
  * @attention 浮点数（Floating-point）：
    浮点数是一种表示实数的数据类型，用于存储和处理包含小数部分的数值。在计算机中，浮点数常用于涉及精确度和范围较大的计算，例如科学计算、工程计算和信号处理等领域。

    arm_rfft_fast_f32()和 arm_rfft_fast_init_f32() 是用于进行快速傅里叶变换（FFT）的主要函数。较旧的函数 arm_rfft_f32() 和 arm_rfft_init_f32() 已被弃用，但仍然有相关的文档。

    对于一个长度为 N 的实数序列的傅里叶变换，其在频域中具有偶对称性。数据的后半部分等于在频域中翻转的前半部分的共轭。通过观察数据，我们发现可以使用 N/2 个复数唯一地表示傅里叶变换结果。这些复数以交替的实部和虚部的方式被压缩到输出数组中：
    X = { real[0], imag[0], real[1], imag[1], real[2], imag[2] ... real[(N/2)-1], imag[(N/2)-1] }

    实际上，第一个复数 (real[0], imag[0]) 实际上是全实数。real[0] 表示直流分量（DC offset），imag[0] 应为0。(real[1], imag[1]) 是基频，(real[2], imag[2]) 是第一个谐波，依此类推。

    实数傅里叶变换函数以这种方式压缩频域数据。正向变换以这种形式输出数据，而逆向变换则要求以这种形式输入数据。该函数始终执行所需的位反转（Bit-reversal），以使输入和输出数据始终按正常顺序排列。
  
    void arm_rfft_fast_f32	(	const arm_rfft_fast_instance_f32 * 	S,
                            float32_t * 	p,
                            float32_t * 	pOut,
                            uint8_t 	ifftFlag 
                            )		
    Parameters
        [in]	S	points to an arm_rfft_fast_instance_f32 structure
        [in]	p	points to input buffer (Source buffer is modified by this function.)
        [in]	pOut	points to output buffer
        [in]	ifftFlag	
                    value = 0: RFFT
                    value = 1: RIFFT
  *
  * Copyright (c) 2023 Zhejiang University.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "fft.h"

// extern const arm_rfft_fast_instance_f32 arm_rfft_fast_sR_f32_len256; // an arm_rfft_fast_instance_f32 structure
// extern const arm_rfft_fast_instance_f32 arm_rfft_fast_sR_f32_len1024;
// extern const arm_rfft_fast_instance_f32 arm_rfft_fast_sR_f32_len2048;
// extern const arm_rfft_fast_instance_f32 arm_rfft_fast_sR_f32_len4096;

// const int POINT_COUNT_1024 = 1024;
// const int POINT_COUNT_2048 = 2048;
// const int POINT_COUNT_4096 = 4096;

// float fft_in_buf[i] = 3 * sin(PI2 * i * 10718 / Fs) + 
//                       2 * sin(PI2 * i * 2143 / Fs) +
//                       1 * sin(PI2 * i * 5360 / Fs) + 2; 


//! incase of truncating the signal
const float hanning_win_table[1024] = {0.00001,0.00004,0.00008,0.00015,0.00023,0.00034,0.00046,0.00060,0.00076,0.00094,0.00114,0.00135,0.00159,0.00184,0.00211,0.00240,0.00271,0.00304,0.00339,0.00375,0.00414,0.00454,0.00496,0.00540,0.00586,0.00634,0.00683,0.00735,0.00788,0.00843,0.00900,0.00959,0.01020,0.01082,0.01146,0.01213,0.01281,0.01350,0.01422,0.01496,0.01571,0.01648,0.01727,0.01808,0.01890,0.01975,0.02061,0.02149,0.02239,0.02330,0.02424,0.02519,0.02616,0.02714,0.02815,0.02917,0.03021,0.03127,0.03235,0.03344,0.03455,0.03568,0.03682,0.03799,0.03917,0.04037,0.04158,0.04281,0.04406,0.04533,0.04661,0.04791,0.04923,0.05057,0.05192,0.05329,0.05467,0.05607,0.05749,0.05893,0.06038,0.06185,0.06333,0.06483,0.06635,0.06788,0.06943,0.07100,0.07258,0.07418,0.07580,0.07743,0.07907,0.08073,0.08241,0.08411,0.08581,0.08754,0.08928,0.09104,0.09281,0.09459,0.09639,0.09821,0.10004,0.10189,0.10375,0.10563,0.10752,0.10943,0.11135,0.11328,0.11523,0.11720,0.11918,0.12117,0.12318,0.12520,0.12723,0.12928,0.13135,0.13342,0.13552,0.13762,0.13974,0.14187,0.14402,0.14618,0.14835,0.15053,0.15273,0.15494,0.15717,0.15941,0.16166,0.16392,0.16619,0.16848,0.17078,0.17310,0.17542,0.17776,0.18011,0.18247,0.18484,0.18723,0.18963,0.19204,0.19446,0.19689,0.19933,0.20179,0.20425,0.20673,0.20922,0.21171,0.21422,0.21674,0.21928,0.22182,0.22437,0.22693,0.22950,0.23209,0.23468,0.23728,0.23989,0.24252,0.24515,0.24779,0.25044,0.25310,0.25577,0.25845,0.26114,0.26384,0.26654,0.26926,0.27198,0.27471,0.27745,0.28020,0.28296,0.28572,0.28850,0.29128,0.29407,0.29687,0.29967,0.30248,0.30530,0.30813,0.31096,0.31380,0.31665,0.31951,0.32237,0.32524,0.32811,0.33099,0.33388,0.33677,0.33967,0.34258,0.34549,0.34841,0.35133,0.35426,0.35720,0.36014,0.36308,0.36603,0.36899,0.37195,0.37491,0.37788,0.38086,0.38384,0.38682,0.38981,0.39280,0.39579,0.39879,0.40180,0.40480,0.40782,0.41083,0.41385,0.41687,0.41989,0.42292,0.42595,0.42898,0.43202,0.43505,0.43809,0.44114,0.44418,0.44723,0.45028,0.45333,0.45638,0.45943,0.46249,0.46555,0.46860,0.47166,0.47472,0.47779,0.48085,0.48391,0.48698,0.49004,0.49310,0.49617,0.49923,0.50230,0.50536,0.50843,0.51149,0.51456,0.51762,0.52068,0.52374,0.52681,0.52987,0.53292,0.53598,0.53904,0.54209,0.54515,0.54820,0.55125,0.55430,0.55734,0.56039,0.56343,0.56647,0.56950,0.57254,0.57557,0.57860,0.58162,0.58464,0.58766,0.59068,0.59369,0.59670,0.59970,0.60271,0.60570,0.60870,0.61169,0.61467,0.61765,0.62063,0.62360,0.62657,0.62953,0.63249,0.63544,0.63839,0.64133,0.64427,0.64720,0.65013,0.65305,0.65597,0.65887,0.66178,0.66467,0.66757,0.67045,0.67333,0.67620,0.67906,0.68192,0.68477,0.68762,0.69046,0.69329,0.69611,0.69893,0.70173,0.70453,0.70733,0.71011,0.71289,0.71566,0.71842,0.72117,0.72392,0.72665,0.72938,0.73210,0.73481,0.73751,0.74021,0.74289,0.74556,0.74823,0.75088,0.75353,0.75617,0.75879,0.76141,0.76402,0.76662,0.76921,0.77178,0.77435,0.77691,0.77946,0.78199,0.78452,0.78703,0.78954,0.79203,0.79451,0.79698,0.79944,0.80189,0.80433,0.80676,0.80917,0.81157,0.81396,0.81634,0.81871,0.82107,0.82341,0.82574,0.82806,0.83037,0.83266,0.83494,0.83721,0.83947,0.84171,0.84395,0.84616,0.84837,0.85056,0.85274,0.85491,0.85706,0.85920,0.86132,0.86343,0.86553,0.86762,0.86969,0.87174,0.87379,0.87581,0.87783,0.87983,0.88182,0.88379,0.88574,0.88769,0.88962,0.89153,0.89343,0.89531,0.89718,0.89904,0.90088,0.90270,0.90451,0.90630,0.90808,0.90984,0.91159,0.91332,0.91504,0.91674,0.91843,0.92010,0.92175,0.92339,0.92501,0.92662,0.92821,0.92978,0.93134,0.93288,0.93441,0.93592,0.93741,0.93889,0.94035,0.94179,0.94322,0.94463,0.94602,0.94740,0.94876,0.95010,0.95143,0.95274,0.95403,0.95531,0.95656,0.95781,0.95903,0.96024,0.96142,0.96260,0.96375,0.96489,0.96601,0.96711,0.96819,0.96926,0.97031,0.97134,0.97236,0.97335,0.97433,0.97529,0.97623,0.97716,0.97807,0.97895,0.97982,0.98068,0.98151,0.98233,0.98313,0.98391,0.98467,0.98541,0.98614,0.98685,0.98754,0.98821,0.98886,0.98949,0.99011,0.99071,0.99129,0.99185,0.99239,0.99291,0.99342,0.99390,0.99437,0.99482,0.99525,0.99566,0.99606,0.99643,0.99679,0.99713,0.99744,0.99774,0.99803,0.99829,0.99853,0.99876,0.99896,0.99915,0.99932,0.99947,0.99960,0.99972,0.99981,0.99988,0.99994,0.99998,1.00000,1.00000,0.99998,0.99994,0.99988,0.99981,0.99972,0.99960,0.99947,0.99932,0.99915,0.99896,0.99876,0.99853,0.99829,0.99803,0.99774,0.99744,0.99713,0.99679,0.99643,0.99606,0.99566,0.99525,0.99482,0.99437,0.99390,0.99342,0.99291,0.99239,0.99185,0.99129,0.99071,0.99011,0.98949,0.98886,0.98821,0.98754,0.98685,0.98614,0.98541,0.98467,0.98391,0.98313,0.98233,0.98151,0.98068,0.97982,0.97895,0.97807,0.97716,0.97623,0.97529,0.97433,0.97335,0.97236,0.97134,0.97031,0.96926,0.96819,0.96711,0.96601,0.96489,0.96375,0.96260,0.96142,0.96024,0.95903,0.95781,0.95656,0.95531,0.95403,0.95274,0.95143,0.95010,0.94876,0.94740,0.94602,0.94463,0.94322,0.94179,0.94035,0.93889,0.93741,0.93592,0.93441,0.93288,0.93134,0.92978,0.92821,0.92662,0.92501,0.92339,0.92175,0.92010,0.91843,0.91674,0.91504,0.91332,0.91159,0.90984,0.90808,0.90630,0.90451,0.90270,0.90088,0.89904,0.89718,0.89531,0.89343,0.89153,0.88962,0.88769,0.88574,0.88379,0.88182,0.87983,0.87783,0.87581,0.87379,0.87174,0.86969,0.86762,0.86553,0.86343,0.86132,0.85920,0.85706,0.85491,0.85274,0.85056,0.84837,0.84616,0.84395,0.84171,0.83947,0.83721,0.83494,0.83266,0.83037,0.82806,0.82574,0.82341,0.82107,0.81871,0.81634,0.81396,0.81157,0.80917,0.80676,0.80433,0.80189,0.79944,0.79698,0.79451,0.79203,0.78954,0.78703,0.78452,0.78199,0.77946,0.77691,0.77435,0.77178,0.76921,0.76662,0.76402,0.76141,0.75879,0.75617,0.75353,0.75088,0.74823,0.74556,0.74289,0.74021,0.73751,0.73481,0.73210,0.72938,0.72665,0.72392,0.72117,0.71842,0.71566,0.71289,0.71011,0.70733,0.70453,0.70173,0.69893,0.69611,0.69329,0.69046,0.68762,0.68477,0.68192,0.67906,0.67620,0.67333,0.67045,0.66757,0.66467,0.66178,0.65887,0.65597,0.65305,0.65013,0.64720,0.64427,0.64133,0.63839,0.63544,0.63249,0.62953,0.62657,0.62360,0.62063,0.61765,0.61467,0.61169,0.60870,0.60570,0.60271,0.59970,0.59670,0.59369,0.59068,0.58766,0.58464,0.58162,0.57860,0.57557,0.57254,0.56950,0.56647,0.56343,0.56039,0.55734,0.55430,0.55125,0.54820,0.54515,0.54209,0.53904,0.53598,0.53292,0.52987,0.52681,0.52374,0.52068,0.51762,0.51456,0.51149,0.50843,0.50536,0.50230,0.49923,0.49617,0.49310,0.49004,0.48698,0.48391,0.48085,0.47779,0.47472,0.47166,0.46860,0.46555,0.46249,0.45943,0.45638,0.45333,0.45028,0.44723,0.44418,0.44114,0.43809,0.43505,0.43202,0.42898,0.42595,0.42292,0.41989,0.41687,0.41385,0.41083,0.40782,0.40480,0.40180,0.39879,0.39579,0.39280,0.38981,0.38682,0.38384,0.38086,0.37788,0.37491,0.37195,0.36899,0.36603,0.36308,0.36014,0.35720,0.35426,0.35133,0.34841,0.34549,0.34258,0.33967,0.33677,0.33388,0.33099,0.32811,0.32524,0.32237,0.31951,0.31665,0.31380,0.31096,0.30813,0.30530,0.30248,0.29967,0.29687,0.29407,0.29128,0.28850,0.28572,0.28296,0.28020,0.27745,0.27471,0.27198,0.26926,0.26654,0.26384,0.26114,0.25845,0.25577,0.25310,0.25044,0.24779,0.24515,0.24252,0.23989,0.23728,0.23468,0.23209,0.22950,0.22693,0.22437,0.22182,0.21928,0.21674,0.21422,0.21171,0.20922,0.20673,0.20425,0.20179,0.19933,0.19689,0.19446,0.19204,0.18963,0.18723,0.18484,0.18247,0.18011,0.17776,0.17542,0.17310,0.17078,0.16848,0.16619,0.16392,0.16166,0.15941,0.15717,0.15494,0.15273,0.15053,0.14835,0.14618,0.14402,0.14187,0.13974,0.13762,0.13552,0.13342,0.13135,0.12928,0.12723,0.12520,0.12318,0.12117,0.11918,0.11720,0.11523,0.11328,0.11135,0.10943,0.10752,0.10563,0.10375,0.10189,0.10004,0.09821,0.09639,0.09459,0.09281,0.09104,0.08928,0.08754,0.08581,0.08411,0.08241,0.08073,0.07907,0.07743,0.07580,0.07418,0.07258,0.07100,0.06943,0.06788,0.06635,0.06483,0.06333,0.06185,0.06038,0.05893,0.05749,0.05607,0.05467,0.05329,0.05192,0.05057,0.04923,0.04791,0.04661,0.04533,0.04406,0.04281,0.04158,0.04037,0.03917,0.03799,0.03682,0.03568,0.03455,0.03344,0.03235,0.03127,0.03021,0.02917,0.02815,0.02714,0.02616,0.02519,0.02424,0.02330,0.02239,0.02149,0.02061,0.01975,0.01890,0.01808,0.01727,0.01648,0.01571,0.01496,0.01422,0.01350,0.01281,0.01213,0.01146,0.01082,0.01020,0.00959,0.00900,0.00843,0.00788,0.00735,0.00683,0.00634,0.00586,0.00540,0.00496,0.00454,0.00414,0.00375,0.00339,0.00304,0.00271,0.00240,0.00211,0.00184,0.00159,0.00135,0.00114,0.00094,0.00076,0.00060,0.00046,0.00034,0.00023,0.00015,0.00008,0.00004,0.00001};

void FFT(float_t *input, float_t *output, uint32_t num)
{
    // float sum = 0;

    // // firstly eliminate the direct part
    // for (int i = 0; i < num; ++i)
    //     sum += input[i];
    // float average = sum / num;

    for (int i = 0; i < num; ++i)
    {
        // input[i] -= average;
        input[i]+=input[i]*hanning_win_table[i];
    }

    // init arm_rfft_fast_instance_f32 structure
    arm_rfft_fast_instance_f32 S;
    arm_rfft_fast_init_f32(&S, num);

    // calculate
    arm_rfft_fast_f32(&S, input, output, 0); // ifftFlag = 0

}

void FFT_Power_Mag(float_t *input, float *mag, uint32_t num)
{
    arm_cmplx_mag_f32(input, mag, num);
    mag[0] /= num;
    for (int i = 1; i < num; i++) 
    {
        mag[i] /= (num / 2);
    }
}
