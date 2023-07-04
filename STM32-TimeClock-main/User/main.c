
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include <stdlib.h>
#include "./led/bsp_led.h" 
#include "./lcd/bsp_ili9341_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "./RTC/bsp_rtc.h"
#include ".\key\bsp_key.h" 
#include "./wind/wind.h"
#include "./irda/bsp_irda.h"
#include "qian_head.h"
#include "./systick/bsp_systick.h"
#include "./beep/bsp_beep.h"

extern uint8_t  frame_flag;
extern uint8_t  isr_cnt;
extern uint8_t  frame_cnt;

#define CLI() __set_PRIMASK(1)		        /* �ر����ж� */  
#define SEI() __set_PRIMASK(0)				/* �������ж� */ 

extern uint8_t  frame_flag;
extern uint8_t  isr_cnt;
extern uint8_t  frame_cnt;

extern int detime,rtime;

int detime,rtime;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */ 
int main(void)
{	
	uint8_t key_val;
    //int runled = 0;
	
  /* LED ��ʼ�� */
	LED_GPIO_Config();
	LED1_ON;
	
    /* ��������ʼ�� */
    BEEP_GPIO_Config();    
//       
    /* flash��ʼ�� */
    SPI_FLASH_Init();
  
	/* ����SysTick Ϊ10us�ж�һ�� */
	SysTick_Init();
	
  /* ��������SysTick���ж����ȼ�Ϊ��ߣ�Ҫ��ȻSysTick��ʱ�ж���ռ����IO EXTI�ж�
   * ��ΪSysTick��ʼ��ʱĬ�����õ����ȼ�����͵�
   * ���ߵ�����������ʱ������ʱ��ʱ��Ҫ���ö�ʱ�������ȼ�����IO EXTI�жϵ����ȼ�
   */
  NVIC_SetPriority (SysTick_IRQn, 0);
  
  
  /* USART1 config 115200 8-N-1 */
//	USART_Config();
	printf("\r\n ����һ������ң�ط��������ʵ�� \r\n");
  
  /* ��ʼ���������ͷCP1838�õ���IO */
	IrDa_Init();  
	
	
  /*=========================Һ����ʼ����ʼ===============================*/
	ILI9341_Init();         //LCD ��ʼ��

	
	//����0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
	//���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
	//���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
  ILI9341_GramScan ( 3 );
  /*=========================Һ����ʼ������===============================*/
  
  /*##-1- Configure the RTC peripheral #######################################*/
  /*����RTCԤ��Ƶ����RTC���ݼĴ��� */
  /*Asynch Prediv  = ��HAL�Զ����� */
  Rtc_Handle.Instance = RTC; 
  Rtc_Handle.Init.AsynchPrediv = RTC_AUTO_1_SECOND;

  if (HAL_RTC_Init(&Rtc_Handle) != HAL_OK)
  {
    /* Initialization Error */
    printf("\r\n RTC��ʼ��ʧ��\r\n");;
  }
  else
	{	
		/* ����Ƿ��Դ��λ */
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
		{
		  printf("\r\n ������Դ��λ....\r\n");
		}
		/* ����Ƿ��ⲿ��λ */
		else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
		{
		  printf("\r\n �����ⲿ��λ....\r\n");
		}
		/* Clear source Reset Flag */
        __HAL_RCC_CLEAR_RESET_FLAGS();
	} 
  
  
	LCD_SetFont(&Font8x16);
	LCD_SetTextColor(WHITE);
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
    
        /*alarm init*/
    Wind_ReadAlarm(&Alarmhour,&Alarmmin);
//    Alarmhour = 1;
//    Alarmmin = 13;
    
    RTC_AlarmSet();
	
//    SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
    
    		/*init the date and time.*/
//    RTC_CalendarConfig();

	for(;;)
	{
        
        if( frame_flag == 1 ) /* һ֡�������ݽ������ */
        {
            key_val = IrDa_Process();
            printf("\r\n key_val=%d \r\n",key_val);
            printf("\r\n ��������frame_cnt=%d \r\n",frame_cnt);
            printf("\r\n �жϴ���isr_cnt=%d \r\1  n",isr_cnt);
            
            
			Alarmflag = 0;
            
            if( key_val == 162 )
                Wind_state = Wind_state == Wind_OFF ? Wind_SHOW : Wind_OFF;
            
            if( Wind_state != Wind_OFF){ 
            
            /* ��ͬ��ң��������Ӧ��ͬ�ļ�ֵ����Ҫʵ�ʲ��� */
            switch( key_val )
            {
                case 0:  
                    //LED1_TOGGLE;
                    printf("\r\n key_val=%d \r\n",key_val);
                    printf("\r\n Error \r\n");
                    break;
                    
                case 162:
                    //LED1_TOGGLE;
                    printf("\r\n key_val=%d \r\n",key_val);
                    printf("\r\n POWER \r\n");
                    break;
                    
                case 226:
                    //LED1_TOGGLE;
                    //runled = !runled;
                    printf("\r\n key_val=%d \r\n",key_val);
                    printf("\r\n MENU \r\n");
                    break;
                
                case 34:
                    //LED1_TOGGLE;
                    Wind_alarmstate = Wind_alarmstate == 0 ? 1 : 0;
                    printf("\r\n key_val=%d \r\n",key_val);
                    printf("\r\n TEST \r\n");
                    break;
                
                
            /********* ��+����  **********/
                case 2:
                    //LED3_TOGGLE;
                    if (Wind_state == Wind_CHANGE)
                    {                      
                        if(Wind_loc == Wind_HOUR)
                            Wind_HourIn(&hours,1);
                        else
                            Wind_MinIn(&minutes,1);
                        Wind_SetTime(hours,minutes);
                        Wind_ChangesIs(&hours , &minutes);
                    }
                    else if ( Wind_state == Wind_ALARM )
                    {       
                        if(Wind_loc == Wind_HOUR)
                            Wind_HourIn(&Alarmhour,1);
                        else
                            Wind_MinIn(&Alarmmin,1);
                        /*��������*/
                        Wind_SetAlarm(Alarmhour,Alarmmin);                            
                    }
                    
    
                    printf("\r\n key_val=%d \r\n",key_val);
                    printf("\r\n + \r\n");
                    break;
                
                case 194:
                    //LED1_TOGGLE;
                    Wind_state = Wind_state == Wind_SHOW ?  Wind_ALARM : Wind_state - 1;
                    printf("\r\n key_val=%d \r\n",key_val);
                    printf("\r\n RETURN \r\n");
                    break;
                
                case 224:
                    //LED1_TOGGLE;
                    
                    Wind_ChangeLoc();
                    
                    printf("\r\n key_val=%d \r\n",key_val);
                    printf("\r\n |<< \r\n");
                    break;
             
                
                
                /*���ż�*/
                case 168:
                    //LED2_TOGGLE;
                    Wind_ChangeState();
                    printf("\r\n key_val=%d \r\n",key_val);
                    printf("\r\n > \r\n");
                    break;
                
                case 144:
                  //LED1_TOGGLE;
                  
                  Wind_ChangeLoc();
                
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n >>| \r\n");
                break;
                
                case 104:
                  //LED1_TOGGLE;
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n 0 \r\n");
                break;
                
                /*��-����*/
                case 152:
                  //LED1_TOGGLE;
                if (Wind_state == Wind_CHANGE)
                    {                      
                        if(Wind_loc == Wind_HOUR)
                            Wind_HourIn(&hours,0);
                        else
                            Wind_MinIn(&minutes,0);
                        Wind_SetTime(hours,minutes);
                        Wind_ChangesIs(&hours , &minutes);
                    }
                    else if ( Wind_state == Wind_ALARM )
                    {       
                        if(Wind_loc == Wind_HOUR)
                            Wind_HourIn(&Alarmhour,0);
                        else
                            Wind_MinIn(&Alarmmin,0);
                                       
                        /*��������*/
                        Wind_SetAlarm(Alarmhour,Alarmmin);                            
                    }
                
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n - \r\n");
                break;
                
                case 176:
                  //LED1_TOGGLE;
                if (Wind_state == Wind_CHANGE)
                    {                      
                        if(Wind_loc == Wind_HOUR)
                            hours = 0;
                        else
                            minutes = 0;
                        Wind_SetTime(hours,minutes);
                        Wind_ChangesIs(&hours , &minutes);
                    }
                    else if ( Wind_state == Wind_ALARM )
                    {       
                        if(Wind_loc == Wind_HOUR)
                            Alarmhour = 0;
                        else
                            Alarmmin = 0;
                                       
                        /*��������*/
                        Wind_SetAlarm(Alarmhour,Alarmmin);                            
                    }
                
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n C \r\n");
                break;
                
                case 48:
                  //LED1_TOGGLE;
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n 1 \r\n");
                break;
                
                case 24:
                  //LED1_TOGGLE;
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n 2 \r\n");
                break;
                
                case 122:
                  //LED1_TOGGLE;
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n 3 \r\n");
                break;
                
                case 16:
                  //LED1_TOGGLE;
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n 4 \r\n");
                break;
                
                case 56:
                  //LED1_TOGGLE;
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n 5 \r\n");
                break;
                
                case 90:
                  //LED1_TOGGLE;
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n 6 \r\n");
                break;
                
                  case 66:
                  //LED1_TOGGLE;
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n 7 \r\n");
                break;
                
                case 74:
                  //LED1_TOGGLE;
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n 8 \r\n");
                break;
                
                case 82:
                  //LED1_TOGGLE;
                  printf("\r\n key_val=%d \r\n",key_val);
                  printf("\r\n 9 \r\n");
                break;
                
                default:       
                break;
            }
            
            if((num = Wind_IsNum(key_val)) != -1)
            {
                Wind_Num();
            }
            }
         
        }
        
        /*ʱ����ʾģʽ*/
		if( Wind_state == Wind_SHOW)
		{
           Wind_TimeShow();
		}
        
        /*ʱ���趨ģʽ*/
		else if (Wind_state == Wind_CHANGE)
		{
			Wind_ChangesIs(&hours , &minutes);
			Wind_ChangeShow(hours,minutes);
		}
        
        /*�����趨ģʽ*/
		else if ( Wind_state == Wind_ALARM )
		{
			Wind_AlarmShow(Alarmhour,Alarmmin);
                 /*������д��flash*/
            Wind_WriteAlarm(&Alarmhour, &Alarmmin);  
//            test(1,AlarmCount);
		}
        else if(Wind_state == Wind_OFF)
        {
            ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
        }            
         

        if(Alarmflag == 1)
        {
            BEEP_ON;
            SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
            LED_RED;
            detime = 20000;
            while(detime){};
            LED_BLUE;
            detime = 10000;
            while(detime);
        }
        else 
        {
            SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
            LED_RGBOFF;
            BEEP_OFF;
        }

        
        if(Wind_state != Wind_OFF){
        if(Wind_alarmstate == 0)
        {
            HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
            ILI9341_DisplayStringEx( 4*48+5 ,4*48+20,20,20,(uint8_t *)"       ",0);
        }
        else
        {
            HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
            ILI9341_DisplayStringEx( 4*48+5 ,4*48+20,20,20,(uint8_t *)"Alarmed",0);
        }
    }
        
    }
}
/*********************************************END OF FILE**********************/

