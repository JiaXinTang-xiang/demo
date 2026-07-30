#ifndef __BEEP_H__
#define __BEEP_H__
#include "stm32f10x.h"                  // Device header

void Beep_alarm(int16_t distance);
void Beep_Init(void);
void Beep_black(void);


#endif
