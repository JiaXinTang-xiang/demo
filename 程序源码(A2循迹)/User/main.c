#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "PWM.h"
#include "Motor.h"
#include "usart2.h"
#include "beep.h"
#include "Delay.h"
#include "usart2.h"
#include "ceju.h"
#include "Xunji.h"
#include "LED.h"
#include "Key.h"
#include "HCSR04.h"

int16_t Black_Num,Flag=0,mode_flag = 1;
uint16_t Distance;

int main(void)
{
	
	/*NVIC中断分组*/ NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //配置NVIC为分组2 即抢占优先级范围：0~3，响应优先级范围：0~3
																	 //此分组配置在整个工程中仅需调用一次,若有多个中断，
																	 //可以把此代码放在main函数内，while循环之前
																	 //若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	/*模块初始化*/
	LED_Init();
	OLED_Init();	 	//OLED初始化
	Motor_Init();    	//电机初始化
	Beep_Init(); 		//蜂鸣器初始化
	xunji_Init();		//循迹初始化 
	Key_Init();			//按键初始化
	HCSR04_Init();	    //超声波初始化
	
	while(1)
	{  
		Distance = HCSR04_GetValue();
		/*2s一次性程序（开始时等待按键按下，若按下则绕行模式，否则正常模式）*/
		if(mode_flag == 1)
		{
		  for(int i=0;i<2;i++)
		  {
			delay_s(1);
			mode_flag = 2;
		  }
	    }
				
		read_sensor();	/*读取红外传感器电平*/
		OLED_ShowNum(1,1,Black_Num,2); /*显示黑块数量*/
		
	/*正常模式（既按键不按下）*/
	if(mode == 0) 
	{	
		if(Flag == 1)	/*停车*/
		{
			Load(-18,-18);
			delay_ms(300);		
			while(1)	
			{
				Load(0,0);
			}
		}
		else if(Flag == 0) /*正常循迹*/
		{			
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);
			if(Distance < 50)
			{
				switch(Distance) //因为超声波不准，所以只要满足这些情况，都停车
			  {
				case 1:case 2:case 3:case 4:case 5:case 6:
				case 7:case 8:case 9:case 10:case 11:case 12:
				case 13:case 14:case 15:case 16:case 17:case 18:
				case 19:case 20:case 21:case 22:case 23:case 24:
				case 25:case 26:case 27:case 28:case 29:case 30:
					LED1_OFF();
					Beep_alarm(Distance);
					Load(0,0);
				break;
				
				default :
					Load(-10,-10);
					Beep_alarm(Distance);
				break;
			  }
			}
			else xunji();
		}
	}
	
	
	/*绕行模式（既按键按下）*/
	else if(mode == 2)
	{
		if(Flag == 1)	/*停车*/
		{
			Load(-18,-18);
			delay_ms(300);		
			while(1)	
			{
				Load(0,0);
			}
		}
		else if(Flag == 0) /*正常循迹*/
		{		
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);	
			if(Distance < 50)
			{
			switch(Distance)		//因为超声波不准，所以只要满足这些情况，都绕行
			  {
				case 1:case 2:case 3:case 4:case 5:case 6:
				case 7:case 8:case 9:case 10:case 11:case 12:
				case 13:case 14:case 15:case 16:case 17:case 18:
				case 19:case 20:case 21:case 22:case 23:case 24:
				case 25:case 26:case 27:
					LED1_OFF();
					Beep_alarm(Distance);
					Load(0,0);
					 
					/*避障*/
					for(int i=0;i<120;i++)//转弯一下
					{
						Load(13,-10);
						delay_ms(5);
					}			
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0)
					{
						GPIO_ResetBits(GPIOB,GPIO_Pin_5);
						Load(-15,-25);
					}
					Load(10,10);
					delay_ms(20);					
				break;
				default :
					Load(-10,-10);
					Beep_alarm(Distance);
				break;
			  }
			}
			else xunji();
		}
	}
  }
} 
