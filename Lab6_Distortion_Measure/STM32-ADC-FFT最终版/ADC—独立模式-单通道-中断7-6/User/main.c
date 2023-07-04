/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include <stdlib.h>
#include "./led/bsp_led.h" 
#include "./adc/bsp_adc.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./systick/bsp_SysTick.h"
#include "./fft/fft.h"
#include "./dsp/stm32_dsp.h"
#include "./dsp/table_fft.h"
#include "string.h"
extern int count;
extern float vol[1024];
float vol[1024];
int count;
extern __IO uint32_t ADC_ConvertedValue;

#define USE_LCD_DISPLAY

//ADC1转换的电压值通过MDA方式传到SRAM


// 局部变量，用于保存转换计算后的电压值 	 
float ADC_Vol; 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{   
		char LCDTemp[10000];
		int flag = 0;
		int j;
		//sprintf(LCDTemp,"OK ");
    /* 配置系统时钟为72 MHz */
    SystemClock_Config();
		LED_GPIO_Config();
	/* 初始化USART1 配置模式为 115200 8-N-1 */
		//SysTick_Init();
    DEBUG_USART_Config();
    Rheostat_Init();    
#ifdef USE_LCD_DISPLAY	
	/*=========================液晶初始化开始===============================*/
	ILI9341_Init();         //LCD 初始化

	//其中0、3、5、6 模式适合从左至右显示文字，
	//不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
	//其中 6 模式为大部分液晶例程的默认显示方向  
  ILI9341_GramScan ( 3 );
  /*=========================液晶初始化结束===============================*/
#endif	
		LCD_SetFont(&Font8x16);
		LCD_SetTextColor(WHITE);
		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
		//Delay_ms(1);
		//SysTick_Delay_Us(1);
    while (count < 1024);
    
    float fx;    
    for(int i=0; i<NPT; i++)
    {
        fx = (1000*vol[i]) * Hanning_Win(i,NPT);   

        lBufInArray[i] = ((signed short)fx) << 16;
    } 
       
    cr4_fft_1024_stm32(lBufOutArray, lBufInArray, NPT); 
    GetPowerMag();

    lBufMagArray[1] = 0;
    lcd_show_fft(lBufMagArray);
       
    lcd_print_fft();
     
    char LCDtemp[1000];
    char LCDout[1000]="";
    for(int j = 0;j<14;j++){
        LCDout[0] = 0;
        for(int i = j*10;i<(j+1)*10;i++){
            sprintf(LCDtemp,"%.2f ", vol[i]);
            strcat(LCDout,LCDtemp);
        }
    //   ILI9341_DispStringLine_EN( LINE(j), LCDout );
    }
    
        
        
        
        
        
        
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
