#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f10x.h"

// PWM参数
#define PWM_PERIOD      20000   // 20ms周期
#define MIN_PULSE       500     // 0.5ms     0度
#define MAX_PULSE       2500    // 2.5ms    180度
#define DEFAULT_PULSE   1500    // 1.5ms    135度

// 运动控制参数
#define SMOOTH_STEP_MS  10   //20   // 10更小的步长，更平滑     // 30较大的步长，更快响应  // 50最大步长，最低CPU负载
#define MOVING_DEADBAND 1    //10   // 1高精度，减少死区        //3 减少抖动              // 5 大死区减少运动次数
#define STATIC_DEADBAND 5    //5    // 3对微小变化敏感          // 8忽略微小变化          // 10忽略大部分微小变化
    
// 函数声明
void SERVO_Init(void);
void SERVO_SetAngle1(uint16_t angle);
void SERVO_SetAngle2(uint16_t angle);
void SERVO_SmoothMove1(uint16_t target_angle, uint16_t duration_ms);
void SERVO_SmoothMove2(uint16_t target_angle, uint16_t duration_ms);

#endif
