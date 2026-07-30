#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "PWM.h"
#include "Motor.h"
#include "beep.h"
#include "Delay.h"
#include "Xunji.h"
#include "LED.h"
#include "Key.h"
#include "HCSR04.h"
#include "Servo.h"

uint16_t Distance,Flag=2,Fahui_flag=0;
int ball=5,ball_fahui=5,id_flag = 0;

int main(void)
{
	
	/*NVIC中断分组*/ NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //配置NVIC为分组2 即抢占优先级范围：0~3，响应优先级范围：0~3
																	 //此分组配置在整个工程中仅需调用一次,若有多个中断，
																	 //可以把此代码放在main函数内，while循环之前
																	 //若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	/*模块初始化*/
	LED_Init();         
	OLED_Init();	  				//OLED初始化	
	Beep_Init(); 					//蜂鸣器初始化
	xunji_Init();					//循迹初始化 
	Key_Init();						//按键初始化
	HCSR04_Init();Timer_Init();	    //超声波初始化
	SERVO_Init();                   //180°舵机初始化
    SERVO_SetAngle(130);			//舵机限幅角度，默认为0
	delay_ms(2000);
	Motor_Init();    				//电机初始化

	while(1)
	{  	
		Distance = Get_Distance();	
		OLED_ShowString(1,1,"Ball:");
		OLED_ShowNum(1,6,ball,2);						//		OLED_ShowNum(1,1,ball,2);	
		GPIO_ResetBits(GPIOA,GPIO_Pin_9);
		
		if(mode == 0)	//基础部分
		{
			switch(Flag) //状态机
			{
				case 0:  //为0(既默认状态时)小车循迹并且测距，当扫到距离<50的物体时，小车停下
				switch(Distance)
				{
					case 1:case 2:case 3:case 4:case 5:case 6:
					case 7:case 8:case 9:case 10:case 11:case 12:
					case 13:case 14:case 15:case 16:case 17:case 18:
					LED1_OFF();        					//灯提示
					Beep_alarm(Distance);				//声提示
					Load(0,0);							//停车
					SERVO_SmoothMove(90,2000);  		//倒球90°
					delay_ms(1000);						//让球倒出去
					SERVO_SetAngle(130);	          	//回到初始状态
					Flag = 3;  							//倒完球重新装球
					break;
					default :xunji();break;
				}
				break;
			
				case 1:/*走过瓶子，防止超声波扫到停止*/
					Load(-15,-15);
					for(int i=0;i<300;i++)
					{
						xunji();
						delay_ms(10);
					}
					Flag = 0;  //走过瓶子后扫瓶子倒球
				break;
				
				case 2:/*开始先打第一个*/
					delay_ms(2000); 			 //等两秒，调整好小车位置
					LED1_OFF();        			 //灯提示
					Beep_alarm(10);				 //声提示
					SERVO_Rotate360_CW();		 //蓄力
					ball-=1;					 //球减一个
					Flag = 3;     				 //打完装球
				break;
		
				case 3:/*装球*/				
					if(ball > 0)           		 //如果还有球
					{
						SERVO_SetAngle(166);
						delay_ms(500);
						SERVO_Rotate_ZQ(400); 	//400	90度
						ball-=1;				//装一次球减一个
						Flag = 1; 				//装完球走过瓶子
					}
					else 
					{
						SERVO_SetAngle(166);	//复位角度
						LED1_ON();        		//关灯
						Load(0,0);
					}
				break;
			}
		}
		
		else if(mode == 1)//发挥部分
		{
			switch(Fahui_flag)
			{
				case 0:
				switch(Distance)
				{
					/*	5-10厘米内用90°倒球  */
					case 5:case 6:case 7:case 8:case 9:case 10:case 11:
					LED1_OFF();        					//灯提示
					Beep_alarm(Distance);				//声提示
					Load(0,0);							//停车
					
						SERVO_SmoothMove(90,2000);  		//倒球90°
						delay_ms(1000);						//让球倒出去
						SERVO_SetAngle(130);	          	//回到初始状态
						Fahui_flag = 2;  	               	//倒完球重新装球
					
					id_flag=0;
					break;
					
					/*	12-16厘米内用100°倒球  */
					case 12:case 13:case 14:case 15:case 16:
					LED1_OFF();        					//灯提示
					Beep_alarm(Distance);				//声提示
					Load(0,0);							//停车
					
						SERVO_SmoothMove(100,2000);  		//倒球90°
						delay_ms(1000);						//让球倒出去
						SERVO_SetAngle(130);	          	//回到初始状态
						Fahui_flag = 2;  					//倒完球重新装球
					id_flag=0;
					break;
					/*	17-25厘米内用150°投球  */
					case 17:case 18:case 19:case 20:case 21:case 22:case 23:
					case 24:case 25:case 26:case 27:case 28:case 29:case 30:
					LED1_OFF();        					//灯提示
					Beep_alarm(Distance);				//声提示
					Load(0,0);							//停车
						SERVO_SetAngle(150);			
						delay_ms(2000); 			 //等两秒，调整好小车位置
						SERVO_Rotate360_CW();		 //蓄力
						//ball-=1;					 //球减一个
						Fahui_flag = 2;     				 //打完装球
					id_flag=0;
					break;
					
										/*	17-25厘米内用150°投球  */
					case 31:case 32:case 33:case 34:case 35:case 36:case 37:
					case 38:case 39:case 40:case 41:case 42:case 43:case 44:
					case 45:case 46:case 47:case 48:case 49:case 50:case 51:	
					LED1_OFF();        					//灯提示
					Beep_alarm(Distance);				//声提示
					Load(0,0);							//停车
					SERVO_SetAngle(130);			
					delay_ms(2000); 			 //等两秒，调整好小车位置
					SERVO_Rotate360_CW();		 //蓄力
					//ball-=1;					 //球减一个
					Fahui_flag = 2;     				 //打完装球
					id_flag=0;
					break;
					
					default :xunji();break;
				}break;
				
				case 1:/*走过瓶子，防止超声波扫到停止*/
					for(int i=0;i<300;i++)
					{
						xunji();
						delay_ms(10);
					}
					Fahui_flag = 0;  //走过瓶子后扫瓶子倒球
				break;
					
				case 2:	/*装球*/				
					if(ball > 0)           		 //如果还有球
					{
						SERVO_SetAngle(166);
						SERVO_Rotate_ZQ(400); 	//400	90度
						ball-=1;				//装一次球减一个
						Fahui_flag = 1; 				//装完球走过瓶子
					}
					else 
					{
						SERVO_SetAngle(166);	//复位角度
						LED1_ON();        		//关灯
						Load(0,0);
					}
				break;
			}
		}
	}	
}

