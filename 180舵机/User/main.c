#include "stm32f10x.h"
#include "Servo.h"
#include "Delay.h"
#include "OLED.h"


int main(void) {
    // 初始化
    OLED_Init();
    SERVO_Init();
    OLED_ShowString(1,1,"180deg Servo Test");
    OLED_ShowString(2,1,"0->90->180->0");
    
//    SERVO_SetAngle1(0);   //PB6,绿色90度
    SERVO_SetAngle2(0);  //PB7,黑色100度

	
                    
    while(1) {
//                 
//		SERVO_SmoothMove1(360, 1000); // 3秒到180
//		Delay_ms(1000);
        SERVO_SmoothMove2(90, 1000); // 3秒到180
        Delay_ms(1000);
//        SERVO_SmoothMove2(0, 1000); // 3秒到180
//		Delay_ms(1000);
//        SERVO_SmoothMove2(60, 1000); // 3秒到180
//        Delay_ms(1000);
    }
}

//爪子张开为0°
//		SERVO_SmoothMove(0, 3000); // 3秒到180
//		Delay_ms(2000);
//爪子抓紧为47°+
//		SERVO_SmoothMove(47, 3000); // 3秒到180
//		Delay_ms(2000);


//抬起时135° 下去时106°
//		SERVO_SmoothMove(135, 3000); // 3秒到180
//		Delay_ms(2000);
//		SERVO_SmoothMov; // 3秒到180
//		Delay_ms(2000);


//伸回来20° 伸出去70°
//		SERVO_SmoothMove(20, 3000); // 3秒到180
//		Delay_ms(2000);
//		SERVO_SmoothMove(70, 3000); // 3秒到180
//		Delay_ms(2000);
