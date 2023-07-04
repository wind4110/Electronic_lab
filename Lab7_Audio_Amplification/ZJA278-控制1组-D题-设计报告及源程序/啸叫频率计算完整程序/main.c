#include  "systemInit.h"
#include  <timer.h>
#include  "lcd12864.h"
#include  <stdio.h>

#define   CCP4_PERIPH     SYSCTL_PERIPH_GPIOF
#define   CCP4_PORT       GPIO_PORTF_BASE
#define   CCP4_PIN        GPIO_PIN_7

//  ��ʱ������������ʼ��
void timer_counter_init(void)
{
  //  ����Timer0Ϊ32λ���ڶ�ʱ������ʱ����Ϊ1s
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER0);                 //  ʹ��Timer0ģ��
  TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);      //  ����Timer0Ϊ32λ���ڶ�ʱ��
  TimerControlStall(TIMER0_BASE,TIMER_A,true);            //  Timer0�ڵ�����������ͣ����
  TimerLoadSet(TIMER0_BASE, TIMER_A, 6000000UL);          //  ����Timer0��ֵ����ʱ1s
  
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);        //  ʹ��Timer0��ʱ�ж�
  IntEnable(INT_TIMER0A);                                 //  ʹ��Timer0�ж�

  //  ����Timer2AΪ16λ������ؼ������������
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER2);                 //  ʹ��Timer2Aģ��
  SysCtlPeriEnable(CCP4_PERIPH);                          //  ʹ��CCP4���ڵ�GPIO�˿�(PF)
  GPIOPinTypeTimer(CCP4_PORT, CCP4_PIN);                  //  ����CCP4�ܽ�(PF7)Ϊ��������
  
  TimerConfigure(TIMER2_BASE, TIMER_CFG_16_BIT_PAIR |     //  ����Timer2AΪ16λ�¼�������
                              TIMER_CFG_A_CAP_COUNT);
  
  TimerControlEvent(TIMER2_BASE,                          //  ����Timer2A����CCP������
                    TIMER_A,
                    TIMER_EVENT_NEG_EDGE);
  
  TimerLoadSet(TIMER2_BASE, TIMER_A, 40000);              //  ���ü�������ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_A, 10000);             //  �����¼�����ƥ��ֵ
  
  TimerIntEnable(TIMER2_BASE, TIMER_CAPA_MATCH);          //  ʹ��Timer2A����ƥ���ж�
  IntEnable(INT_TIMER2A);                                 //  ʹ��Timer2A�ж�
  TimerEnable(TIMER2_BASE, TIMER_A);                      //  ʹ��Timer2A����
  
  IntMasterEnable();                                      //  ʹ�ܴ������ж�
}

//  ��������������ڣ�
int main(void)
{
    jtagWait();                                             //  ��ֹJTAGʧЧ����Ҫ��
    clockInit();                                            //  ʱ�ӳ�ʼ��������6MHz
    timer_counter_init();                                   //  ��ʱ������������ʼ��
    softspi_init();                                         //  lcd��ʾ���IO�ĳ�ʼ��
    lcd_init();                                             //  lcd��ʼ��
    
    TimerEnable(TIMER0_BASE, TIMER_A);                      //  ʹ��Timer0����

    for (;;)
    {
    }
}


//  ��ʱ�����жϷ�����
void Timer0A_ISR(void)
{
    unsigned long ulStatus;
    unsigned long countervalue;
    unsigned long frequency;
    char c[20];
    
    ulStatus = TimerIntStatus(TIMER0_BASE, true);           //  ��ȡ�ж�״̬
    TimerIntClear(TIMER0_BASE, ulStatus);                   //  ����ж�״̬����Ҫ��

    if (ulStatus & TIMER_TIMA_TIMEOUT)                      //  �����Timer0��ʱ�ж�
    {
      countervalue = TimerValueGet(TIMER2_BASE,TIMER_A);    //  �õ��������ĵ�ǰֵ
      TimerLoadSet(TIMER2_BASE, TIMER_A, 40000);            //  ���ü�������ֵ
      frequency = 40000 - countervalue;                     //  ���㷽��Ƶ��
      
      sprintf(c, "Ƶ��:%ldHz", frequency);
      lcd_write(com,0x01);	                            //  ��������DDRAM�ĵ�ַ����������
      Show(0x90,c);
    }
}

//  ���������жϺ���
void Timer2A_ISR(void)
{
    unsigned long ulStatus;

    ulStatus = TimerIntStatus(TIMER2_BASE, true);           //  ��ȡ��ǰ�ж�״̬
    TimerIntClear(TIMER2_BASE, ulStatus);                   //  ����ж�״̬����Ҫ��

    if (ulStatus & TIMER_CAPA_MATCH)                        //  ����Timer2A����ƥ���ж�
    {
        ;
    }
}

