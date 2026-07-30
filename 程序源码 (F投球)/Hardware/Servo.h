#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f10x.h"

// 硬件配置（PB4对应TIM3_CH1）
#define SERVO_TIM       TIM3
#define SERVO_PORT      GPIOB
#define SERVO_PIN       GPIO_Pin_4

// PWM参数
#define PWM_PERIOD      20000   // 20ms周期
#define MIN_PULSE       500     // 0.5ms
#define MAX_PULSE       2500    // 2.5ms
#define DEFAULT_PULSE   1500    // 1.5ms

// 运动控制参数(180°用的)
#define SMOOTH_STEP_MS 20
#define MOVING_DEADBAND 2
#define STATIC_DEADBAND 5

// 运动控制参数(360°用的)
#define CW_START_PWM    1550    // 启动脉宽（比停止值高50us）
#define CW_FULL_PWM     1700    // 全速脉宽
#define STOP_PWM        1500    // 停止脉宽
#define ROTATION_MS     4100    // 旋转一圈所需时间（需实测校准）
#define ZHUANGQIU_MS     1000    // 装球所需时间（需实测校准）

// 全局变量声明
extern uint16_t current_angle;
extern uint8_t is_moving;

void SERVO_Init(void);
void SERVO_SetAngle(uint16_t angle);
void SERVO_SmoothMove(uint16_t target_angle, uint16_t duration_ms);
void SERVO_Rotate360_CW(void);
void SERVO_Rotate_ZQ(uint16_t zhuangqiu_ms);

#endif


