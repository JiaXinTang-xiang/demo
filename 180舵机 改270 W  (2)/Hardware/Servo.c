#include "Servo.h"
#include "Delay.h"
#include "stm32f10x.h"


void SERVO_Init(void) {
    // 1. 时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // 2. 配置PB6和PB7为复用推挽
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // 3. 时基配置 (PWM)
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_InitStruct.TIM_Prescaler = 72 - 1;       // 72MHz/72=1MHz
    TIM_InitStruct.TIM_Period = 20000 - 1;       // 20ms周期
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_InitStruct);
      
    // 4. PWM通道配置 
    TIM_OCInitTypeDef OC_InitStruct;
    TIM_OCStructInit(&OC_InitStruct);
    OC_InitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    OC_InitStruct.TIM_OutputState = TIM_OutputState_Enable;
    OC_InitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

    // 通道1 (PB6)  
    OC_InitStruct.TIM_Pulse = 0; // 初始0度   改1500就是135度
    TIM_OC1Init(TIM4, &OC_InitStruct);
    
    // 通道2 (PB7)
    OC_InitStruct.TIM_Pulse = 0;
    TIM_OC2Init(TIM4, &OC_InitStruct);
    
    // 5. 使能预装载
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    
    // 6. 启动定时器
    TIM_Cmd(TIM4, ENABLE);
}

void SERVO_SetAngle1(uint16_t angle) {
    // 角度限幅
    if (angle > 270) angle = 270;
  
    // 计算脉冲宽度
    uint16_t pulse = MIN_PULSE + (angle * (MAX_PULSE - MIN_PULSE)) / 270;
    TIM_SetCompare1(TIM4, pulse); 
}

void SERVO_SetAngle2(uint16_t angle) {
    if (angle > 270) angle = 270;
    uint16_t pulse = MIN_PULSE + (angle * (MAX_PULSE - MIN_PULSE)) / 270;
    TIM_SetCompare2(TIM4, pulse); 
}

void SERVO_SmoothMove1(uint16_t target_angle, uint16_t duration_ms) {
    if (duration_ms < 20) return;
    if (target_angle > 270) target_angle = 270;
    
    // 获取当前实际角度（从硬件读取）
    uint16_t current_pulse = TIM_GetCapture1(TIM4);
    uint16_t current_angle = (current_pulse - MIN_PULSE) * 270 / (MAX_PULSE - MIN_PULSE);
    
    uint16_t steps = duration_ms / SMOOTH_STEP_MS;
    
    float current = current_angle; // 从实际角度开始
    float step = (float)(target_angle - current_angle) / steps;
    
    for (uint16_t i = 0; i < steps; i++) {
        current += step;
        SERVO_SetAngle1((uint16_t)(current + 0.5f)); // 四舍五入
        Delay_ms(SMOOTH_STEP_MS);
    }
    
    // 确保到达目标角度
    SERVO_SetAngle1(target_angle);
}

void SERVO_SmoothMove2(uint16_t target_angle, uint16_t duration_ms) {
    if (duration_ms < 20) return;
    if (target_angle > 270) target_angle = 270;
    
    // 获取当前实际角度
    uint16_t current_pulse = TIM_GetCapture2(TIM4);
    uint16_t current_angle = (current_pulse - MIN_PULSE) * 270 / (MAX_PULSE - MIN_PULSE);
    
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