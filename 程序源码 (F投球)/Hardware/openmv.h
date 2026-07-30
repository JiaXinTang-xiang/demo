#ifndef __OPENMV_H
#define __OPENMV_H

#include <stdio.h>

typedef struct {
    uint8_t class_id;
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
} OpenMV_Data;

//extern char Serial2_RxPacket[];
extern uint8_t Serial2_RxPacket[100];
uint8_t Serial_GetRxFlag(void);
extern uint8_t Serial_Rx;
extern int angle;
extern int Y;
extern OpenMV_Data mv_data;  // 声明全局变量（不分配内存）

void Serial2_Init(void);
void Serial2_SendByte(uint8_t Byte);
void Serial2_SendArray(uint8_t *Array, uint16_t Length);
void Serial2_SendString(char *String);
void Serial2_SendNumber(uint32_t Number, uint8_t Length);
void Serial2_Printf(char *format, ...);
void Xian_Get(int* angle ,float* y);
void ParseOpenMVData(void);
uint8_t Serial_GetRx(void);
uint16_t Openmv_Fahui(void);



#endif
