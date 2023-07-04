#include  "systemInit.h"
#include  <timer.h>
#include  "lcd12864.h"
#include  <stdio.h>

#define   CCP4_PERIPH     SYSCTL_PERIPH_GPIOF
#define   CCP4_PORT       GPIO_PORTF_BASE
#define   CCP4_PIN        GPIO_PIN_7

//  定时器及计数器初始化
void timer_counter_init(void)
{
  //  配置Timer0为32位周期定时器，定时周期为1s
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER0);                 //  使能Timer0模块
  TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);      //  配置Timer0为32位周期定时器
  TimerControlStall(TIMER0_BASE,TIMER_A,true);            //  Timer0在单步调试下暂停运行
  TimerLoadSet(TIMER0_BASE, TIMER_A, 6000000UL);          //  设置Timer0初值，定时1s
  
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);        //  使能Timer0超时中断
  IntEnable(INT_TIMER0A);                                 //  使能Timer0中断

  //  配置Timer2A为16位输入边沿计数捕获计数器
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER2);                 //  使能Timer2A模块
  SysCtlPeriEnable(CCP4_PERIPH);                          //  使能CCP4所在的GPIO端口(PF)
  GPIOPinTypeTimer(CCP4_PORT, CCP4_PIN);                  //  配置CCP4管脚(PF7)为脉冲输入
  
  TimerConfigure(TIMER2_BASE, TIMER_CFG_16_BIT_PAIR |     //  配置Timer2A为16位事件计数器
                              TIMER_CFG_A_CAP_COUNT);
  
  TimerControlEvent(TIMER2_BASE,                          //  控制Timer2A捕获CCP负边沿
                    TIMER_A,
                    TIMER_EVENT_NEG_EDGE);
  
  TimerLoadSet(TIMER2_BASE, TIMER_A, 40000);              //  设置计数器初值
  TimerMatchSet(TIMER2_BASE, TIMER_A, 10000);             //  设置事件计数匹配值
  
  TimerIntEnable(TIMER2_BASE, TIMER_CAPA_MATCH);          //  使能Timer2A捕获匹配中断
  IntEnable(INT_TIMER2A);                                 //  使能Timer2A中断
  TimerEnable(TIMER2_BASE, TIMER_A);                      //  使能Timer2A计数
  
  IntMasterEnable();                                      //  使能处理器中断
}

//  主函数（程序入口）
int main(void)
{
    jtagWait();                                             //  防止JTAG失效，重要！
    clockInit();                                            //  时钟初始化：晶振，6MHz
    timer_counter_init();                                   //  定时器及计数器初始化
    softspi_init();                                         //  lcd显示相关IO的初始化
    lcd_init();                                             //  lcd初始化
    
    TimerEnable(TIMER0_BASE, TIMER_A);                      //  使能Timer0计数

    for (;;)
    {
    }
}


//  定时器的中断服务函数
void Timer0A_ISR(void)
{
    unsigned long ulStatus;
    unsigned long countervalue;
    unsigned long frequency;
    char c[20];
    
    ulStatus = TimerIntStatus(TIMER0_BASE, true);           //  读取中断状态
    TimerIntClear(TIMER0_BASE, ulStatus);                   //  清除中断状态，重要！

    if (ulStatus & TIMER_TIMA_TIMEOUT)                      //  如果是Timer0超时中断
    {
      countervalue = TimerValueGet(TIMER2_BASE,TIMER_A);    //  得到计数器的当前值
      TimerLoadSet(TIMER2_BASE, TIMER_A, 40000);            //  设置计数器初值
      frequency = 40000 - countervalue;                     //  计算方波频率
      
      sprintf(c, "频率:%ldHz", frequency);
      lcd_write(com,0x01);	                            //  清屏，将DDRAM的地址计数器归零
      Show(0x90,c);
    }
}

//  计数器的中断函数
void Timer2A_ISR(void)
{
    unsigned long ulStatus;

    ulStatus = TimerIntStatus(TIMER2_BASE, true);           //  读取当前中断状态
    TimerIntClear(TIMER2_BASE, ulStatus);                   //  清除中断状态，重要！

    if (ulStatus & TIMER_CAPA_MATCH)                        //  若是Timer2A捕获匹配中断
    {
        ;
    }
}

