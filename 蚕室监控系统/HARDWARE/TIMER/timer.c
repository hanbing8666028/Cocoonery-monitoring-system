#include "timer.h"
#include "led.h"
#include "beep.h"
extern u8 beep_count;
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//定时器 驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
u8 time_485flag;
//定时器3中断服务程序
void TIM3_IRQHandler(void)
{
    if(TIM3->SR&0X0001)//溢出中断
    {
        LED1=!LED1;
        time_485flag = 1;
        if (beep_count > 0)
        {
            beep_count--;
            BEEP = ~BEEP;
        }

    }
    TIM3->SR&=~(1<<0);//清除中断标志位
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    RCC->APB1ENR|=1<<1; //TIM3时钟使能
    TIM3->ARR=arr;      //设定计数器自动重装值//刚好1ms
    TIM3->PSC=psc;      //预分频器7200,得到10Khz的计数时钟
    TIM3->DIER|=1<<0;   //允许更新中断
    TIM3->CR1|=0x01;    //使能定时器3
  MY_NVIC_Init(1,3,TIM3_IRQn,2);//抢占1，子优先级3，组2
}














