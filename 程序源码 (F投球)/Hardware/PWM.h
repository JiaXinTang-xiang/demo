#ifndef __PWM_H
#define __PWM_H
#include "stm32f10x.h"                  // Device header


void PWM_Init_TIM1(void);
void PWM_SetCompare1(int32_t Compare);
void PWM_SetCompare4(int32_t Compare);

#endif
