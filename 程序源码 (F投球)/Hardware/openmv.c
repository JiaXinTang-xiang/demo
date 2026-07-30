#include "stm32f10x.h"
#include "openmv.h"
#include <stdarg.h>
#include "OLED.h"
#include "Motor.h"

uint8_t Serial2_RxPacket[100];
uint8_t Serial_Rx;
int angle;
int Y;
OpenMV_Data mv_data;
extern int id_flag;

// 数据解析函数
void ParseOpenMVData(void)
{
    mv_data.class_id = Serial2_RxPacket[1];
    mv_data.x   = (Serial2_RxPacket[3] << 8) | Serial2_RxPacket[2];
    mv_data.y   = (Serial2_RxPacket[5] << 8) | Serial2_RxPacket[4];
    mv_data.width  = (Serial2_RxPacket[7] << 8) | Serial2_RxPacket[6];
    mv_data.height = (Serial2_RxPacket[9] << 8) | Serial2_RxPacket[8];
}


void Serial2_Init(void)
{
    // 时钟配置
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // USART2→USART3
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // GPIOA→GPIOB

    // GPIO配置（PB10/PB11）
    GPIO_InitTypeDef GPIO_InitStructure;
    // TX引脚（PB10）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // RX引脚（PB11）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART配置
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStructure); 

    // 断通道
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; // 中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART3, ENABLE); 
}

void Serial2_SendByte(uint8_t Byte)
{
    USART_SendData(USART3, Byte); 
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET); 
}


//发送数组
void Serial2_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i ++)
    {
        Serial2_SendByte(Array[i]);
    }
}
//发送字符串
void Serial2_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i ++)
    {
        Serial2_SendByte(String[i]);
    }
}
//X的Y次方
uint32_t Serial2_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y --)
    {
        Result *= X;
    }
    return Result;
}
//发送数字
void Serial2_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i ++)
    {
        Serial2_SendByte(Number / Serial2_Pow(10, Length - i - 1) % 10 + '0');
    }
}
//printf重定向
int fputc2(int ch, FILE *f)
{
    Serial2_SendByte(ch);
    return ch;
}
//对sprintf的封装
void Serial2_Printf(char *format, ...)
{
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    Serial2_SendString(String);
}

//少了两个，加了一个，区别在这
void Xian_Get(int* angle, float* y)
{
    int i;
    *angle = 0;
    *y = 0;
    for(i=0; Serial2_RxPacket[i] != '#'; i++) {
        *angle = *angle * 10 + (Serial2_RxPacket[i] - '0');
    }
    for(i++; Serial2_RxPacket[i] != '\0'; i++) {
        *y = *y * 10 + (Serial2_RxPacket[i] - '0');
    }
}
//加
uint8_t Serial_GetRx(void)
{
    if (Serial_Rx == 1) {
        Serial_Rx = 0; // 清除标志位
        return 1;
    } 
    return 0;
}


// Serialopenmv.c
// 修改6：中断服务函数名
void USART3_IRQHandler(void)  // USART2→USART3
{
    static uint8_t RxState = 0;
    static uint8_t pRxPacket = 0;
    
    if(USART_GetITStatus(USART3, USART_IT_RXNE)) // USART2→USART3
    {
        uint8_t RxData = USART_ReceiveData(USART3); // USART2→USART3

        switch(RxState)
        {
        case 0: // 等待'@'
            if(RxData == '@')
            {
                RxState = 1;
                pRxPacket = 0;
                Serial2_RxPacket[pRxPacket++] = RxData; // 存储@
            }
            break;
            
        case 1: // 接收后续11字节
            Serial2_RxPacket[pRxPacket++] = RxData;
            if(pRxPacket >= 12) // 收到完整12字节
            {
                RxState = 2;
            }
            break;
            
        case 2: // 验证结束符
            if(Serial2_RxPacket[10] == '\r' && 
               Serial2_RxPacket[11] == '\n')
            {
                Serial_Rx = 1; // 设置接收标志
            }
            RxState = 0; // 重置状态机
            break;
        }
        USART_ClearITPendingBit(USART3, USART_IT_RXNE); // USART2→USART3
    }
}
	
uint16_t Openmv_Fahui(void)
{
	USART_Cmd(USART3, ENABLE);
	
	if(Serial_GetRx())
    {
		  // 验证协议头尾
		 if(Serial2_RxPacket[0] == '@' && 
			Serial2_RxPacket[10] == '\r' && 
			Serial2_RxPacket[11] == '\n')
		{
			ParseOpenMVData();  // 调用数据解析函数

		}
		  OLED_ShowNum(2, 11, mv_data.class_id, 1);//识别到红色为0，没有识别到为1
		  OLED_ShowNum(2, 3, mv_data.x, 3);
		  OLED_ShowNum(2, 3, mv_data.x, 3);
		
			if(mv_data.class_id == 1)
			{
				if(mv_data.x!=159)//假设1是坐标
				{
					mv_data.x>159 ? Load(20,20):Load(-20,-20);
				}
				else
				{		
					Load(0,0);
					id_flag = 1;
				}
			}	
	}	
//	USART_Cmd(USART3, DISABLE);
	return id_flag;
	
}
