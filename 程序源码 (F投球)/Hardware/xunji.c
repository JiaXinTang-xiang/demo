#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "PWM.h"
#include "Motor.h"
#include "usart2.h"
#include "beep.h"
#include "Delay.h"
#include "LED.h"

int16_t zleft=0,left=0,mid=0,right=0,zright=0;

void xunji_Init(void)
{
    // 开启GPIOA和GPIOB时钟
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
    // 配置PA5, PA6, PA7为浮空输入（无上拉/下拉）
	  GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 配置PB0, PB1为浮空输入  
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void read_sensor(void)//红外传感器识别到黑线返回数字信号低电平0，未识别到黑线返回高电平1
{
	zright = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);  // PA5
    right = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);  // PA6
    mid = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);  // PA7
    left = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);  // PB0
    zleft = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);  // PB1	
	
}

void xunji()
{
	read_sensor();
	LED1_ON();
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	if(zleft == 0 && left == 1 && mid == 0 && right == 0 && zright == 0)	//小左转
	{
		Load(-3,-20);
	}	
	else if(zleft == 0 && left == 1 && mid == 1 && right == 0 && zright == 0)	//小小左转
	{
		Load(10,-18);
	}		
	else if(zleft == 0 && left == 0 && mid == 1 && right == 0 && zright == 0)	//直走1
	{
		Load(-18,-17);
	}
	else if(zleft == 0 && left == 0 && mid == 1 && right == 1 && zright == 0)	//小小右转
	{
		Load(-18,-10);
	}
	else if(zleft == 0 && left == 0 && mid == 0 && right == 1 && zright == 0)	//小右转
	{
		Load(-20,-2);
	}	

	
	else if(zleft == 1 && left == 0 && mid == 0 && right == 0 && zright == 0)	//大左转
	{
		Load(-2,-15);
	}
	else if(zleft == 0 && left == 0 && mid == 0 && right == 0 && zright == 1)	//大右转
	{
		Load(-15,-2);
	}	
}
