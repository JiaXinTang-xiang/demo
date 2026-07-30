#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "PWM.h"
#include "Motor.h"
#include "usart2.h"
#include "beep.h"
#include "Delay.h"
#include "usart2.h"
#include "ceju.h"
#include "LED.h"

int16_t M_flag=0,Z_flag=1,R_flag=0,Stop_flag=50; // 有一些定义了但是后面没用上
int16_t zleft=0,left=0,mid=0,right=0,zright=0,Black_flag=0;
extern int16_t Black_Num;
extern int16_t Flag;

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
	
	
	/*满足这些情况都停车*/
	if(zleft == 1 && left == 0 && mid == 1 && right == 0 && zright == 1)
	Flag = 1;
	else if(zleft == 1 && left == 1 && mid == 1 && right == 1 && zright == 1)
	Flag = 1;
	else if(zleft == 0 && left == 1 && mid == 0 && right == 1 && zright == 0)
	Flag = 1;	
}

void xunji()
{
	LED1_ON();
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	if(zleft == 1 && left == 0 && mid == 0 && right == 0 && zright == 0) //大左转
	{
		Load(-15,-30);
	}	
	else if(zleft == 0 && left == 1 && mid == 0 && right == 0 && zright == 0) //小左转
	{
		Load(-21,-25);
		if(Black_flag ==1)Black_flag=2;
	} 
	else if(zleft == 0 && left == 1 && mid == 1 && right == 0 && zright == 0)	//小小左转
	{
		Load(-22,-24);
	}
	else if(zleft == 0 && left == 0 && mid == 1 && right == 0 && zright == 0)	//直走1
	{
		Load(-24,-23);
		if(Black_flag ==1)Black_flag=2;	
	}		

	else if(zleft == 0 && left == 0 && mid == 1 && right == 1 && zright == 0)	//小小右转
	{
		Load(-25,-20);
		if(Black_flag ==1)Black_flag=2;	
	}
	else if(zleft == 0 && left == 0 && mid == 0 && right == 1 && zright == 0)	//小右转
	{ 
		Load(-30,-10
		
		
		);
	}
		else if(Black_flag==0 && zleft == 0 && left == 1 && mid == 1 && right == 1 && zright == 0) //识别黑块1
	{ 	
		LED1_OFF();
		Black_Num=1;
		Black_flag=1;
		Beep_black();
	}
	else if(Black_flag==2 && zleft == 0 && left == 1 && mid == 1 && right == 1 && zright == 0) //识别黑块2
	{ 	
		LED1_OFF();
		Black_Num=2;
		Black_flag=3;
		Beep_black();
	}
	else if(zright == 1)	//大右转
	{ 
		for(int i=0;i<130;i++)//直走一下
		{
			Load(-18,-18);
			delay_ms(4);
		}
		for(int i=0;i<150;i++)//转弯一下
		{
			Load(13,-10);
			delay_ms(5);
		}
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)  
		{
			Load(14,-10);
		}
		Load(10,10);
		delay_ms(20);
	}	
}
