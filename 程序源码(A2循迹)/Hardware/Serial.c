#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "OLED.h"

char Serial_RxPacket[100];				//"@MSG\r\n"
uint8_t Serial_RxFlag;
uint16_t fore,back,left,right;

void Serial_Init(void)
{
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//配置gpio口
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART初始化
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	//开启RXNE到中断的标志位
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//NVIC的通道配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//NVIC初始化
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	//开启开关
	USART_Cmd(USART1, ENABLE);
}
//发送字节
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}
//发送数组
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}
//发送字符串
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}
//X的Y次方
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}
//发送数字
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}
//printf重定向
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}
//对sprintf的封装
void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

//USART的中断
void USART1_IRQHandler(void)
	{                            //利用状态机来接受数据包  
//	static uint8_t RxState = 0;//状态位
	static uint8_t pRxPacket = 0;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		Serial_RxPacket[pRxPacket] = RxData;
        Serial_RxFlag = 1;
//		if (RxState == 0)
//		{
//			if (RxData == '@' && Serial_RxFlag == 0)
//			{
//				RxState = 1;
//				pRxPacket = 0;
//			}
//		}
//		else if (RxState == 1)
//		{
//			if (RxData =='\r')
//			{
//				RxState = 2;
//			}
//			else
//			{
//				Serial_RxPacket[pRxPacket] = RxData;
//				pRxPacket ++;
//			}
//		}
//		else if (RxState == 2)
//		{
//			if (RxData == '\n')
//			{
//				RxState = 0;
//				Serial_RxPacket[pRxPacket] = '\0';
//				Serial_RxFlag = 1;
//			}
//		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}



void LANYA(void)
{
 if (Serial_RxFlag == 1)		//如果接收到数据包
		{
			/*将收到的数据包与预设的指令对比，以此决定将要执行的操作*/
//               if (strcmp(Serial_RxPacket, "0") == 0)			
//			{
//				 fore=0,back=0,left=0,right=0;

//			}
//			else if (strcmp(Serial_RxPacket, "2") == 0)			
//			{
//				 fore=1,back=0,left=0,right=0;

//			}
//           else if (strcmp(Serial_RxPacket, "8") == 0)			
//			{
//				 fore=0,back=1,left=0,right=0;

//			}
//           else if (strcmp(Serial_RxPacket, "6") == 0)			
//			{
//				 fore=0,back=0,left=1,right=0;

//			}
//            else if (strcmp(Serial_RxPacket, "4") == 0)			
//			{
//				 fore=0,back=0,left=0,right=1;

//			}
//          else
//               fore=0,back=0,left=0,right=0;
    
      if(Serial_RxPacket[0]==0x00)
    {
       fore=0,back=0,left=0,right=0;
    }
    else if(Serial_RxPacket[0]==0x02)
{
  fore=1,back=0,left=0,right=0;
}
   else  if(Serial_RxPacket[0]==0x08)
    {
       fore=0,back=1,left=0,right=0;
    }
    else if(Serial_RxPacket[0]==0x04)
{
  fore=0,back=0,left=1,right=0;
}
 else if(Serial_RxPacket[0]==0x06)
{
  fore=0,back=0,left=0,right=1;
}

//        OLED_ShowChar(4,10,'k');
			Serial_RxFlag = 0;			//处理完成后，需要将接收数据包标志位清零，否则将无法接收后续数据包
		}
}




