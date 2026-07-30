#include "Servo.h"
#include "Delay.h"
#include "stm32f10x.h"


void SERVO_Init(void) {
    // 1. 时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 改用TIM3

    // 2. 配置PB0和PB1为复用推挽
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // 3. 时基配置 (PWM)
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_InitStruct.TIM_Prescaler = 72 - 1;       // 72MHz/72=1MHz
    TIM_InitStruct.TIM_Period = 20000 - 1;       // 20ms周期
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_InitStruct);     // 使用TIM3
      
    // 4. PWM通道配置 
    TIM_OCInitTypeDef OC_InitStruct;
    TIM_OCStructInit(&OC_InitStruct);
    OC_InitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    OC_InitStruct.TIM_OutputState = TIM_OutputState_Enable;
    OC_InitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

    // 通道3 (PB0)  
    OC_InitStruct.TIM_Pulse = 1500; // 初始中位（90°）
    TIM_OC3Init(TIM3, &OC_InitStruct);
    
    // 通道4 (PB1)
    OC_InitStruct.TIM_Pulse = 1500;
    TIM_OC4Init(TIM3, &OC_InitStruct);
    
    // 5. 使能预装载
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    
    // 6. 启动定时器
    TIM_Cmd(TIM3, ENABLE);
}

void SERVO_SetAngle1(uint16_t angle) {
    // 角度限幅
    if (angle > 180) angle = 180;
  
    // 计算脉冲宽度
    uint16_t pulse = MIN_PULSE + (angle * (MAX_PULSE - MIN_PULSE)) / 180;
    TIM_SetCompare1(TIM3, pulse); 
}

// 设置舵机2（PB1，TIM3_CH4）
void SERVO_SetAngle2(uint16_t angle) {
    if (angle > 180) angle = 180;
    uint16_t pulse = MIN_PULSE + (angle * (MAX_PULSE - MIN_PULSE)) / 180;
    TIM_SetCompare4(TIM3, pulse); // 改为TIM3_CH4
}

// 舵机1平滑移动（PB0，TIM3_CH3）
void SERVO_SmoothMove1(uint16_t target_angle, uint16_t duration_ms) {
    if (duration_ms < 20) return;
    if (target_angle > 180) target_angle = 180;
    
    // 获取当前实际角度（从硬件读取）
    uint16_t current_pulse = TIM_GetCapture3(TIM3); // 改为TIM3_CH3
    uint16_t current_angle = (current_pulse - MIN_PULSE) * 180 / (MAX_PULSE - MIN_PULSE);
    
    uint16_t steps = duration_ms / SMOOTH_STEP_MS;
    float current = current_angle;
    float step = (float)(target_angle - current_angle) / steps;
    
    for (uint16_t i = 0; i < steps; i++) {
        current += step;
        SERVO_SetAngle1((uint16_t)(current + 0.5f));
        Delay_ms(SMOOTH_STEP_MS);
    }
    
    SERVO_SetAngle1(target_angle);
}

// 舵机2平滑移动（PB1，TIM3_CH4）
void SERVO_SmoothMove2(uint16_t target_angle, uint16_t duration_ms) {
    if (duration_ms < 20) return;
    if (target_angle > 180) target_angle = 180;
    
    // 获取当前实际角度
    uint16_t current_pulse = TIM_GetCapture4(TIM3); // 改为TIM3_CH4
    uint16_t current_angle = (current_pulse - MIN_PULSE) * 180 / (MAX_PULSE - MIN_PULSE);
    
    uint16_t steps = duration_ms / SMOOTH_STEP_MS;
    float current = current_angle;
    float step = (float)(target_angle - current_angle) / steps;
    
    for (uint16_t i = 0; i < steps; i++) {
        current += step;
        SERVO_SetAngle2((uint16_t)(current + 0.5f));
        Delay_ms(SMOOTH_STEP_MS);
    }
    
    SERVO_SetAngle2(target_angle);
}
