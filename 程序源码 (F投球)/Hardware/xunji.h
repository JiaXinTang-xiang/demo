#ifndef __XUNJI_H__
#define __XUNJI_H__

#include "stm32f10x.h"                  // Device header

void xunji_Init(void);
void read_sensor(void);
void xunji(void);
extern int16_t zleft,left,mid,right,zright,R_flag;

#endif


