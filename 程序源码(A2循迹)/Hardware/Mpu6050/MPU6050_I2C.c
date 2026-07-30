#include "MPU6050_I2C.h"
 
void MPU6050_IIC_IO_Init(void)
{
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO ,ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	 //释放PB3和PB4引脚
	
	My_GPIO_Init(MPU6050_IIC_GPIO,MPU6050_IIC_SCL_Pin|MPU6050_IIC_SDA_Pin,GPIO_TW_OUT,GPIO_P_NO,GPIO_50MHz);//推挽输出 不拉 50m
	
	//My_GPIO_Init(
	//MPU6050_IIC_GPIO,                // GPIOB
	//MPU6050_IIC_SCL_Pin | MPU6050_IIC_SDA_Pin,  // PB3 + PB4
	//GPIO_KL_OUT,                     // 开漏输出（关键修改！）
	//GPIO_P_NO,                       // 不上下拉（硬件已有上拉）
	//GPIO_50MHz                       // 高速模式
	//);
	//	
	MPU6050_IIC_SCL=1;
	MPU6050_IIC_SDA=1;
}

void MPU6050_IIC_SDA_IO_OUT(void)
{
	My_GPIO_Init(MPU6050_IIC_GPIO,MPU6050_IIC_SDA_Pin,GPIO_TW_OUT,GPIO_P_NO,GPIO_50MHz);//推挽输出 不拉 50m	
//	My_GPIO_Init(
//    MPU6050_IIC_GPIO,
//    MPU6050_IIC_SDA_Pin,
//    GPIO_KL_OUT,   // 开漏输出（关键修改！）
//    GPIO_P_NO,     // 不启用内部上下拉（外部已有上拉）
//    GPIO_50MHz     // 高速模式（可选 GPIO_10MHz）
//	);
}

void MPU6050_IIC_SDA_IO_IN(void)
{
	//My_GPIO_Init(MPU6050_IIC_GPIO,MPU6050_IIC_SDA_Pin,GPIO_FK_IN,GPIO_P_UP,GPIO_50MHz);//浮空输入 上拉 50m
	
	My_GPIO_Init(
	MPU6050_IIC_GPIO,
	MPU6050_IIC_SDA_Pin,
	GPIO_FK_IN,    // 浮空输入（或 GPIO_IPU 更保险）
	GPIO_P_UP,     // 启用内部上拉（如果外部无上拉）
	GPIO_50MHz     // 速度不影响输入模式
    );	
}
//产生MPU6050_IIC起始信号
void MPU6050_IIC_Start(void)
{
	  MPU6050_IIC_SDA_IO_OUT();
		MPU6050_IIC_SDA=1;	  	  
		MPU6050_IIC_SCL=1;
		MPU6050_IIC_delay_4us();
		MPU6050_IIC_SDA=0; 
		MPU6050_IIC_delay_4us();
		MPU6050_IIC_SCL=0; 
}	  
//产生MPU6050_IIC停止信号
void MPU6050_IIC_Stop(void)
{
	  MPU6050_IIC_SDA_IO_OUT();
		MPU6050_IIC_SCL=0;
		MPU6050_IIC_SDA=0;   
		MPU6050_IIC_delay_4us();
		MPU6050_IIC_SCL=1; 
		MPU6050_IIC_delay_4us(); 
		MPU6050_IIC_SDA=1;   
		MPU6050_IIC_delay_4us();  					   	
	}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 MPU6050_IIC_Read_Ack(void)
{
		u8 ucErrTime=0;
	  MPU6050_IIC_SDA_IO_IN();
		MPU6050_IIC_SDA=1;
	  MPU6050_IIC_delay_4us();	   
		MPU6050_IIC_SCL=1;
	  MPU6050_IIC_delay_4us();	 
		while(MPU6050_IIC_SDA_IN)
		{
			ucErrTime++;
			if(ucErrTime>250)
			{
				MPU6050_IIC_Stop();
				return 1;
			}
		}
		MPU6050_IIC_SCL=0; 	   
		return 0;  
} 
//发送ACK应答
void MPU6050_IIC_Send_Ack(u8 ack)
{
    MPU6050_IIC_SDA_IO_OUT();  // 设置为输出
    MPU6050_IIC_SCL = 0;
    MPU6050_IIC_SDA = ack;      // 0:ACK, 1:NACK
    MPU6050_IIC_delay_4us();
    MPU6050_IIC_SCL = 1;
    MPU6050_IIC_delay_4us();
    MPU6050_IIC_SCL = 0;
    MPU6050_IIC_SDA_IO_IN();    // 切换回输入（释放总线）
}				 				     
//MPU6050_IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void MPU6050_IIC_Send_Byte(u8 txd)
{                        
    u8 t;
	  MPU6050_IIC_SDA_IO_OUT();	
    MPU6050_IIC_SCL=0; 
    for(t=0;t<8;t++)
    {              
			if((txd&0x80)>>7)
				MPU6050_IIC_SDA=1;
			else
				MPU6050_IIC_SDA=0;
			txd<<=1; 	  
			MPU6050_IIC_delay_4us();    
			MPU6050_IIC_SCL=1;
			MPU6050_IIC_delay_4us(); 
			MPU6050_IIC_SCL=0;	
			MPU6050_IIC_delay_4us();
    }
    MPU6050_IIC_Read_Ack();		
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 MPU6050_IIC_Read_Byte(u8 ack)
{
		unsigned char i,receive=0;
		MPU6050_IIC_SDA_IO_IN();
		for(i=0;i<8;i++ )
		{
			MPU6050_IIC_SCL=0; 
			MPU6050_IIC_delay_4us();
			MPU6050_IIC_SCL=1;
			receive<<=1;
			if(MPU6050_IIC_SDA_IN)receive++;   
			MPU6050_IIC_delay_4us(); 
		}					 
		MPU6050_IIC_SCL=0;    
		MPU6050_IIC_delay_4us();
		MPU6050_IIC_Send_Ack(ack);
		return receive;
}


