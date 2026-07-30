#include "stm32f10x.h"                  // Device header
#include "usart2.h"
#include "Delay.h"

void send_com(u8 function,u8 value )
{
	u8 bytes[4]={0};
	bytes[0]=0xa5;
	bytes[1]=function;//功能指令
	bytes[2]=value;//指令值
	USART_Send(bytes,4);//发送帧头、功能字节、校验和
}


