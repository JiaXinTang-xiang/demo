#ifndef __USART2_H__
#define __USART2_H__
#include "stm32f10x.h"                  // Device header
#include "sys.h"


void usart2_init(u32 bound);
void USART_Send_Data(char data);
void USART_Send_String(char *String);
int fputc(int ch, FILE *f);
//¶ÔsprintfµÄ·â×°
void Serial_Printf(char *format, ...);

void send_3out(uint8_t *data,uint8_t length,uint8_t send);
void send_out(int16_t *data,uint8_t length,uint8_t send);
void display(int16_t *num,u8 send,u8 count);
void CHeck(uint8_t *re_data);
void USART_Send(uint8_t *Buffer, uint8_t Length);

void USART2_IRQHandler(void);

extern u8 re_Buf_Data[9],Receive_ok;

#endif

