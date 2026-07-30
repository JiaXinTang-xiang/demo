#include "stm32f10x.h"                  // Device header
#include "HCSR04.h"
#include "Delay.h"

int count=0;
int N=0;
float dis=0;

void Timer_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//选择APB1总线下的定时器Timer2
	
	TIM_InternalClockConfig(TIM2);		//TIM2使用内部时钟
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		//计数模式，此处为向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 50000 -1;		//ARR 1 = 0.0001S
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 -1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;		//高级计时器特有，重复计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清除 TIM1 的中断待处理位，先清除，再开启中断
	TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_Trigger, ENABLE);//开启计数器中断（定时器1，计数|触发中断源，使能）
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;		//中断通道选择
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//优先级，同上	
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);		//打开定时器
}

void TIM2_IRQHandler()		//定时器2的中断函数，不懂直接套用
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{

		if (GPIO_ReadInputDataBit(Echo_Port, Echo_Pin) == 1)
		{
			N += 1;	//每过500毫秒N+1	  
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		//清空标志位
	}
}

void HCSR04_Init()
{
	RCC_APB2PeriphClockCmd(Trig_RCC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = Trig_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Trig_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = Echo_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Echo_Port, &GPIO_InitStruct);
	
//	GPIO_ResetBits(Trig_Port, Trig_Pin);
	
}

float Get_Distance(void)
{
	    GPIO_ResetBits(GPIOA, Trig_Pin);//预先拉低Trig引脚
		GPIO_SetBits(GPIOA, Trig_Pin);
		delay_us(20); //信号需要超过10微秒
		GPIO_ResetBits(GPIOA, Trig_Pin);//发出10us的脉冲
	
		while(GPIO_ReadInputDataBit(GPIOA,Echo_Pin)==0)
		{}
			 
    	//等待信号发出，信号发出，计数器置0, 信号发出，变为高电平，引脚置1，进行下一步，否则一直进行while的空循环

	    TIM2->CNT=0;//us级别读取，cnt计数器以微秒为单位计次，更精确
	
		while(GPIO_ReadInputDataBit(GPIOA,Echo_Pin) == 1); 
		//等待信号接受，信号发出过程中，引脚一直置位1 ,当没接收到信号时，
		//引脚一直是1，不会进行下一步，接收到信号时引脚变0，进行下一步
	
	
	    //count=N;//引脚从发出到接受的过程中N一直再加，所以就能判断出时间,当溢出时间以微秒为单位时可用N的计数方法
		count=TIM2->CNT;
	

		//单位cm
		//v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us
		//s = vt/2 = t*0.034/2 = t*0.017 ≈ t/58
		
		dis=(float)count/58;					
		return dis;
}
