#include "stm32f10x.h"
#include "Delay.h"

void Key_Init(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 改为GPIOA
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_6; // PA1和PA6
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  // 初始化GPIOA
}

uint8_t Key_GetNum(void)
{
    uint8_t KeyNum = 0;
    
    /* 检测PA1按键 */
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)     // PA1按下
    {
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0); // 等待松开
        Delay_ms(20);
        KeyNum = 1;  // 键值1对应PA1
    }
    
    /* 检测PA6按键 */
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)     // PA6按下
    {
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0); // 等待松开
        Delay_ms(20);
        KeyNum = 2;  // 键值2对应PA6
    }
    
    return KeyNum;
}
