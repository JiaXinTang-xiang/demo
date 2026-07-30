#ifndef __HCSR04_H__
#define __HCSR04_H__
#include "stm32f10x.h"                  // Device header


#define Trig_Port 		GPIOA
#define Trig_Pin 		GPIO_Pin_2
#define Trig_RCC		RCC_APB2Periph_GPIOA
#define Echo_Port 		GPIOA
#define Echo_Pin 		GPIO_Pin_3
#define Echo_RCC		RCC_APB2Periph_GPIOA

void Timer_Init(void);
void HCSR04_Init(void);
uint16_t HCSR04_GetValue(void);

#endif

