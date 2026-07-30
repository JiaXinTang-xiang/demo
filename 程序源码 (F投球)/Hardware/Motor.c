#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "sys.h"


#define Ain1  PBout(14)
#define Ain2  PBout(15)

#define Bin1  PBout(12)
#define Bin2  PBout(13)


int PWM_MAX=100,PWM_MIN=100;	// PWM限幅变量

void Motor_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);							
	
	PWM_Init_TIM1();											
}


//限幅函数
//入口参数：电机A和B的脉冲个数
void Limit(int *motoA,int *motoB)
{
	if(*motoA > PWM_MAX)*motoA = PWM_MAX;
	if(*motoA < PWM_MIN)*motoA = PWM_MIN;
	
	if(*motoB > PWM_MAX)*motoB = PWM_MAX;
	if(*motoB < PWM_MIN)*motoB = PWM_MIN;
}

//绝对值函数
//入口参数：常规变量
int abs(int p)
{
	int q;
	q=p>0?p:(-p);
	return q;
}

//赋值函数
//入口参数即为PID运算完成后最终的PWM值
void Load(int moto1,int moto2)
{
	//1.正负号对应正反转
	if(moto1>0)
		Ain1 = 1,Ain2 = 0;//正转
	else
		Ain1 = 0,Ain2 = 1;//反转
	
	//2.研究Pwm值
	TIM_SetCompare1(TIM1,abs(moto1));
	
	
	if(moto2>0)
		Bin1 = 1,Bin2 = 0;//正转
	else
		Bin1 = 0,Bin2 = 1;//反转
	
	//2.研究Pwm值
	TIM_SetCompare4(TIM1,abs(moto2));	
}






