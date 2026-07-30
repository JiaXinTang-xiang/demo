#include "Servo.h"
#include "Delay.h"

uint16_t current_angle = 90;
uint8_t is_moving = 0;
static uint16_t current_pulse = DEFAULT_PULSE;

void SERVO_Init(void) {
   
    // 1. 时钟使能（关键步骤）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 2. 禁用JTAG并重映射TIM3
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  // 禁用JTAG
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);      // TIM3_CH1重映射到PB4
	
    // 3. 配置PB4为复用推挽输出
	GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  // 必须为复用推挽
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 4. 定时器基础配置
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_TimeBaseStructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_Prescaler = 72 - 1;       // 72MHz/72=1MHz
    TIM_InitStruct.TIM_Period = 20000 - 1;       // 20ms周期
    TIM_TimeBaseInit(TIM3, &TIM_InitStruct);

    // 5. PWM通道配置（TIM3_CH1）
	TIM_OCInitTypeDef OC_InitStruct;
    OC_InitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    OC_InitStruct.TIM_OutputState = TIM_OutputState_Enable;
    OC_InitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    OC_InitStruct.TIM_Pulse = current_pulse;
    TIM_OC1Init(TIM3, &OC_InitStruct);  // 通道1
	TIM_OC2Init(TIM3, &OC_InitStruct);  // 通道2
	TIM_OC4Init(TIM3, &OC_InitStruct);  // 通道4
    // 6. 使能预装载
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}


/*直接旋转(度数)*/
void SERVO_SetAngle(uint16_t angle) 
{
    angle = (angle > 180) ? 180 : angle;
    current_angle = angle;
    current_pulse = MIN_PULSE + (angle * (MAX_PULSE - MIN_PULSE)) / 180;
    TIM_SetCompare1(SERVO_TIM, current_pulse); 
}



/*平滑旋转（度数,多少时间内转完）*/
void SERVO_SmoothMove(uint16_t target_angle, uint16_t duration_ms) 
{
    if(duration_ms < 20) return;
    
    target_angle = (target_angle > 180) ? 180 : target_angle;
    uint16_t steps = duration_ms / SMOOTH_STEP_MS;
    is_moving = 1;
    
    float current = current_angle;
    float step = (target_angle - current) / steps;
    
    for(uint16_t i=0; i<steps; i++) {
        current += step;
        SERVO_SetAngle((uint16_t)(current + 0.5f));
        delay_ms(SMOOTH_STEP_MS);
    }
    
    is_moving = 0;
    SERVO_SetAngle(target_angle);
}


/*旋转360°*/
void SERVO_Rotate360_CW(void) 
	{
    /* 三阶段控制 */
    // 1. 加速到全速（1500→1700）
    for(uint16_t pwm = CW_START_PWM; pwm <= CW_FULL_PWM; pwm += 10) {
        TIM_SetCompare2(TIM3, pwm);  
        delay_ms(20);
    }
    
    // 2. 保持全速（精确控制旋转时间）
    delay_ms(ROTATION_MS);
    
    // 3. 减速停止（1700→1500）
    for(uint16_t pwm = CW_FULL_PWM; pwm >= STOP_PWM; pwm -= 10) {
        TIM_SetCompare2(TIM3, pwm);  // 通道2
        delay_ms(20);
    }
    
    // 4. 最终稳定（可选）
    delay_ms(50);
}

void SERVO_Rotate_ZQ(uint16_t zhuangqiu_ms) 
	{
    /* 三阶段控制 */
    // 1. 加速到全速（1500→1700）
    for(uint16_t pwm = CW_START_PWM; pwm <= CW_FULL_PWM; pwm += 10) {
        TIM_SetCompare4(TIM3, pwm);  
        delay_ms(20);
    }
    
    // 2. 保持全速（精确控制旋转时间）
    delay_ms(zhuangqiu_ms);
    
    // 3. 减速停止（1700→1500）
    for(uint16_t pwm = CW_FULL_PWM; pwm >= STOP_PWM; pwm -= 10) {
        TIM_SetCompare4(TIM3, pwm);  // 通道2
        delay_ms(20);
    }
    
    // 4. 最终稳定（可选）
    delay_ms(50);
}
