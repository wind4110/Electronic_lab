/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "hmi_driver.h"
#include "hmi_user_uart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_BUFFER_LENGTH 1024
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
  uint8_t Counter_Freq = 1; // 4 pins
  uint8_t Selecter_Voltage = 1; // 3 pins
  uint8_t Output_Mode = 0; // 0 for 4 pins, 1 for 3 pins
  uint16_t ADC_Value[ADC_BUFFER_LENGTH];
  uchar Res = 0x1A;
  uchar Res_High = 0x7F;
  uchar Res_Low = 0x00;
  // uint16_t ADC_1, ADC_2;
  // uint8_t i;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/** 按键按下标置宏
* 按键按下为高电平，设置 KEY_ON=1， KEY_OFF=0
* 若按键按下为低电平，把宏设置成KEY_ON=0 ，KEY_OFF=1 即可
*/
#define KEY_ON  1
#define KEY_OFF 0

/**
* @brief   检测是否有按键按下
* @param  GPIOx:具体的端口, x可以是（A...G）
* @param  GPIO_PIN:具体的端口位， 可以是GPIO_PIN_x（x可以是0...15）
* @retval  按键的状态
*     @arg KEY_ON:按键按下
*     @arg KEY_OFF:按键没按下
*/
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
    /*检测是否有按键按下 */
    if (HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEY_ON ) {
        /*等待按键释放 */
        while (HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEY_ON);
        return  KEY_ON;
    } else
        return KEY_OFF;
}
int fputc(int ch,FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xFFFF);
	return ch;
}

int Set_Signal_Value(int set_value, int ADC_value, uchar *address, uchar *address_high, uchar *address_low)
{
    int set_value_mid = set_value * 40960 / 33;
    int set_value_min = set_value_mid - 100;
    int set_value_max = set_value_mid + 100;
    int address_mid = (*address_high + * address_low)/2;
    if ((ADC_value > set_value_max)&&((*address_high) > (*address_low)))
    {
        (*address_high) = *address - 1;
        (*address) = ((*address_low) + (*address))/2;
        printf("too high");
    }
    else if ((ADC_value < set_value_min)&&((*address_high) > (*address_low)))
    {
        (*address_low) = *address + 1;
        (*address) = ((*address_high)+(*address))/2;
        printf("too low");
    }
    else
    {
        (*address) = (*address_low);
        printf("just");
        return 0;
    }
}

void Lissajous_Figures(uint16 *xdata, uint16 *ydata, uint16 x0, uint16 y0, uint16 N)
{

  // for (int i = 0;i < N-2; i++)
  // {
  //   GUI_Line(xdata[i]/16 + x0, ydata[i]/16 + y0, xdata[i + 2]/16 + x0, ydata[i + 2]/16 + y0);
  // }
  for (int i = 0; i < N - 3; i += 2)
  {
    uint16 x_start = ADC_Value[i] / 16 + x0;
    uint16 y_start = ADC_Value[i + 1] / 16 + y0;
    // uint16 x_end = ADC_Value[i + 2] / 16 + x0;
    // uint16 y_end = ADC_Value[i + 3] / 16 + y0;
    // GUI_Line(x_start, y_start, x_end, y_end);
    GUI_Dot(x_start, y_start);
  }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* EXTI line interrupt detected */
  if (GPIO_Pin == GPIO_PIN_0)
  {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
    if (Output_Mode == 0)
    {
        Counter_Freq = (Counter_Freq) % 5 + 1;
        printf("Counter_Freq = %d, Selecter_Voltage = %d, Output_Mode = %d\r\n", Counter_Freq, Selecter_Voltage,
               Output_Mode); // 0 for Freq, 1 for Voltage
    }
    else if (Output_Mode == 1)
    {
        Selecter_Voltage = (Selecter_Voltage) % 3 + 1;
        Res = 0x1A;
        Res_High = 0x7F;
        Res_Low = 0x00;
        printf("Counter_Freq = %d, Selecter_Voltage = %d, Output_Mode = %d\r\n", Counter_Freq, Selecter_Voltage,
               Output_Mode); // 0 for Freq, 1 for Voltage
    }
  }
  if (GPIO_Pin == GPIO_PIN_13)
  {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
    Output_Mode = 1 - Output_Mode;
    printf("Counter_Freq = %d, Selecter_Voltage = %d, Output_Mode = %d\r\n", Counter_Freq, Selecter_Voltage,
           Output_Mode); // 0 for Freq, 1 for Voltage
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  * 需求：根据ADC_Value[ADC_BUFFER_LENGTH] 得到的一个数组，实现模拟xy模式双通道示波器画图。其中，x通道采样的值存放在ADC_Value[0],ADC_Value[2]这样的偶数下标位置，y通道采样得到数值存放在奇数下标位置。
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  TPL0401A_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  SetBcolor(0x0000);
  SetFcolor(0x0000); // black
  GUI_RectangleFill(50,200,50+300,200+300);
  SetFcolor(0x07E0); // light green
	
  while (1)
  {
    // if (Key_Scan(GPIOA, GPIO_PIN_0) == KEY_ON)
    // {
    //   if (Output_Mode == 1){
    //     Counter_Freq = (Counter_Freq) % 5 + 1;
    //     printf("Counter_Freq = %d, Selecter_Voltage = %d, Output_Mode = %d\r\n", Counter_Freq, Selecter_Voltage, Output_Mode); // 0 for Freq, 1 for Voltage
    //   }
    //   else if(Output_Mode == 0)
    //   {
    //     Selecter_Voltage = (Selecter_Voltage)% 3 + 1;
    //     Res = 0x1A;
    //     Res_High = 0x7F;
    //     Res_Low = 0x00;
    //     printf("Counter_Freq = %d, Selecter_Voltage = %d, Output_Mode = %d\r\n", Counter_Freq, Selecter_Voltage, Output_Mode); // 0 for Freq, 1 for Voltage
    //   }
    // }
    
    
    // if (Key_Scan(GPIOC, GPIO_PIN_13) == KEY_ON) // Key2, Set Mode
    // {
    //   Output_Mode = 1 - Output_Mode;
    //   printf("Counter_Freq = %d, Selecter_Voltage = %d, Output_Mode = %d\r\n", Counter_Freq, Selecter_Voltage, Output_Mode); // 0 for Freq, 1 for Voltage
    // }
    switch (Counter_Freq)
    {
    case 1:
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 0);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 0);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 0);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);
      break;
    case 2:
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 1);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 1);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 1);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);
      break;
    case 3:
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 0);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 1);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 1);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);
      break;
    case 4:
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 0);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 1);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 1);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);
      break;
    case 5:
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 1);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 0);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 1); 
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);
      break;  
    }

    // ADC_Value = malloc(ADC_BUFFER_LENGTH * sizeof(uint16_t));
    ADC_ConversionStop_Disable(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_Value, ADC_BUFFER_LENGTH);
    HAL_Delay(10);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1); // LED_Blue
    // printf("welcome to a new world! \r\n");
    // SetTextValue(0,1,(uchar *)"hello,world!");
    // SetTextInt32(0,5,ADC_Value[0],0,4);
    // for (int i = 0; i < ADC_BUFFER_LENGTH;)
    // {
    //     ADC_1 = ADC_Value[i++];
    //     ADC_2 = ADC_Value[i++];
    // }
    // printf("  double channel ADC test\r\n");
    // printf("ADC_Value[0] is %d\r\n", ADC_Value[0]);
    // printf("ADC_Value[1] is %d\r\n", ADC_Value[1]);
    // printf("PC0 = %1.4f V\r\n", ADC_1*3.3f/4096);
    // printf("PC1 = %1.4f V\r\n", ADC_2*3.3f/4096);
    // MX_DMA_DeInit();
    // HAL_ADC_MspDeInit(&hadc1);
    
    // HAL_ADC_Stop_DMA(&hadc1);
    

    // HAL_ADC_DeInit(&hadc1);
    // printf("ADC is stopped!\r\n");
    int PC0_Max = ADC_Value[0], PC1_Max = 0;
    int PC0_Min = ADC_Value[0], PC1_Min = 4095;
    int PC0_Vpp = 0;
    int PC1_Vpp = 0;

    // printf("Res = %2x, Res_Low = %2x, Res_High = %2x\r\n", Res, Res_Low, Res_High);
    for (int i = 0; i < ADC_BUFFER_LENGTH; i += 2)
    {
    // printf("ADC_Value[%4d]=%4d,ADC_Value[%4d]=%4d.    PC0_Max=%4d,PC1_Max=%4d,    PC0_Min=%4d,PC1_Min=%4d.    PC0_Vpp=%4d,PC1_Vpp=%4d.   Res=%2x\r\n", i, ADC_Value[i], i + 1, ADC_Value[i + 1],PC0_Max,PC1_Max,PC0_Min,PC1_Min,PC0_Vpp,PC1_Vpp,Res);
      if (ADC_Value[i]> PC0_Max)
      {
        PC0_Max = ADC_Value[i];
      }
      if (ADC_Value[i+1]> PC1_Max)
      {
        PC1_Max = ADC_Value[i+1];
      }
      if (ADC_Value[i]< PC0_Min)
      {
        PC0_Min = ADC_Value[i];
      }
      if (ADC_Value[i+1]< PC1_Min)
      {
        PC1_Min = ADC_Value[i+1];
      }
      PC0_Vpp = PC0_Max - PC0_Min;
      PC1_Vpp = PC1_Max - PC1_Min;
    }
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // LED_Green

    Set_Signal_Value(Selecter_Voltage, PC1_Vpp, &Res, &Res_High, &Res_Low);
    Set_TPL0401A_Value(Res);

    Lissajous_Figures(&ADC_Value[0],&ADC_Value[1],50,200,ADC_BUFFER_LENGTH);
    
    // free(ADC_Value);
    // void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
