#include "stm32f10x.h"                  // Device header
#include "Delay.h"


int16_t mode;

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_Structure;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_Structure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);
	
	// 3. 配置PA4为EXTI4中断源
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);

    // 4. 配置EXTI4为下降沿触发
	EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
	
	// 5. 配置NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 优先级设置
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//uint8_t Key_GetNum(void)
//{
//	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == 0)
//	{
//		delay_ms(20);
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == 0);
//		delay_ms(20);
//		KeyNum = 2;
//	}
//	
//	return KeyNum;
//}

// EXTI4中断服务函数


void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {        
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
        {
		   for(volatile uint32_t i = 0; i < 5000; i++);   //中断中不能用delay，否则会卡死程序
		   while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == 0);
		   for(volatile uint32_t i = 0; i < 5000; i++);
		   mode = 1;
        }
        EXTI_ClearITPendingBit(EXTI_Line4); // 清除中断标志
    }
}
