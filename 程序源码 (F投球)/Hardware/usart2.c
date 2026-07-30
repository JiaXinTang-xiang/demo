#include "stm32f10x.h"                  // Device header
#include "string.h"
#include <stdio.h>
#include <stdarg.h>

u8 re_Buf_Data[9]={0},Receive_ok=0;
void usart2_init(u32 bound)
{
	//GPIO端口设置
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   // 时钟GPIOB，USART3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  
   //USART3_TX   PA2
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART3_RX	  PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	//USART 初始化设置 
	USART_InitTypeDef USART_InitStructure;	
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
    USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART2, ENABLE);                    //使能串口 
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;       // EXTI5~EXTI9 共享的中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级（根据需求调整）
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;        // 子优先级（根据需求调整）
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;           // 使能中断
	NVIC_Init(&NVIC_InitStruct);
	
}


// 发送一个
void USART2_Send_Data(char data)
{
  USART_SendData(USART2,data);
  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==0);  // 除非发送完成  
}

// 发送一串
void USART2_Send_String(char *String)
{
  u16 len,j;
  len=strlen(String);
  for(j=0;j<len;j++)
  {
    USART2_Send_Data(*String++);
  }
}

//printf重定向
int fputc(int ch, FILE *f)
{
	USART2_Send_Data(ch);
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
	USART2_Send_String(String);
}

//发送多字节数据+校验和
void USART_Send(uint8_t *Buffer, uint8_t Length)
{
	uint8_t i=0;
	while(i<Length)
	{
		if(i<(Length-1))
		Buffer[Length-1]+=Buffer[i];//累加Length-1前的数据
		USART2_Send_Data(Buffer[i++]);
	}
}
void send_3out(uint8_t *data,uint8_t length,uint8_t send)
{
	uint8_t TX_DATA[30]={0},i=0,k=0;
	TX_DATA[i++]=0X5A;//帧头
	TX_DATA[i++]=0X5A;//帧头
	TX_DATA[i++]=send;//功能字节
	TX_DATA[i++]=length;//数据个数
	for(k=0;k<length;k++)//存入数据到缓存TX_DATA
	{
		TX_DATA[i++]=(uint8_t)data[k];
	}
	USART_Send(TX_DATA,length+5);	
}
//发送一帧数据
void send_out(int16_t *data,uint8_t length,uint8_t send)
{
	uint8_t TX_DATA[30],i=0,k=0;
	memset(TX_DATA,0,(2*length+5));//清零缓存TX_DATA
	TX_DATA[i++]=0X5A;//帧头
	TX_DATA[i++]=0X5A;//帧头
	TX_DATA[i++]=send;//功能字节
	TX_DATA[i++]=2*length;//数据个数
	for(k=0;k<length;k++)//存入数据到缓存TX_DATA
	{
		TX_DATA[i++]=(uint16_t)data[k]>>8;
		TX_DATA[i++]=(uint16_t)data[k];
	}
	USART_Send(TX_DATA,2*length+5);	
}
	void display(int16_t *num,u8 send,u8 count)
{
	  u8 i=0;
		USART2_Send_Data(0X0d);
		USART2_Send_Data(0X0a);
		USART2_Send_Data(send);
	  USART2_Send_Data(':');
		for(i=0;i<count;i++)
		{
			if(num[i]<0)
			{
				num[i]=-num[i];
				USART2_Send_Data('-');
			}
			else
				USART2_Send_Data('+');
		
			USART2_Send_Data(0x30|(num[i]/10000));	
			USART2_Send_Data(0x30|(num[i]%10000/1000));
			USART2_Send_Data(0x30|(num[i]%1000/100));
			//USART_send_byte(0x2e);
			USART2_Send_Data(0x30|(num[i]%100/10));
			USART2_Send_Data(0x30|(num[i]%10));
			USART2_Send_Data(',');	
	}
}
void CHeck(uint8_t *re_data)
{
	
}


void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	static uint8_t i=0,rebuf[20]={0};
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//判断接收标志
	{
		rebuf[i++]=USART_ReceiveData(USART2);//读取串口数据，同时清接收标志
		if (rebuf[0]!=0x5a)//帧头不对
			i=0;	
		if ((i==2)&&(rebuf[1]!=0x5a))//帧头不对
			i=0;
	
		if(i>3)//i等于4时，已经接收到数据量字节rebuf[3]
		{
			if(i!=(rebuf[3]+5))//判断是否接收一帧数据完毕
				return;	
			switch(rebuf[2])//接收完毕后处理
			{
				case 0x15:
					if(!Receive_ok)//当数据处理完成后才接收新的数据
					{
						memcpy(re_Buf_Data,rebuf,9);//拷贝接收到的数据
						Receive_ok=1;//接收完成标志
					}
					break;
			
			}
			i=0;//缓存清0
		}
	}
} 


