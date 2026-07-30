#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Beep_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使用RCC开启GPIO
	
	GPIO_InitTypeDef GPIO_InitStructure;//结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//(设置模式)推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//设置端口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置输出速度
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
}

void Beep_alarm(int16_t distance)
{
//		distance*=5;
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		delay_ms(distance);
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		delay_ms(distance);
}

void Beep_black(void)
{
	for(int i=0;i<50;i++)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		delay_ms(1);
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		delay_ms(1);
	}
}






