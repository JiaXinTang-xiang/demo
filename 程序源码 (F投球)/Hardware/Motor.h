#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f10x.h"                  // Device header
#include "sys.h"


void Motor_Init(void);
void Limit(int *motoA,int *motoB);
int abs(int p);
void Load(int moto1,int moto2);

#endif
