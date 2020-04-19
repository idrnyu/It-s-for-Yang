#ifndef __DHT11_H
#define __DHT11_H

#include "main.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define IODHT11_GPIO_Port GPIOC

//IO方向设置
#define DHT11_IO_IN()  {IODHT11_GPIO_Port->CRH &= 0xFF0FFFFF;	IODHT11_GPIO_Port->CRH |= 8 << 20;}	// PC13 IN  MODE
#define DHT11_IO_OUT() {IODHT11_GPIO_Port->CRH &= 0xFF0FFFFF;	IODHT11_GPIO_Port->CRH |= 3 << 20;}	// PC13 OUT MODE

////IO操作函数
#define	DHT11_DQ_OUT PCout(13) //数据端口	PC13
#define	DHT11_DQ_IN  PCin(13)  //数据端口	PC13

u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u8 *temp, u8 *humi); //读取温湿度
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11
u8 DHT11_Read_Data_Float(float *temp,float *humi);

#endif

