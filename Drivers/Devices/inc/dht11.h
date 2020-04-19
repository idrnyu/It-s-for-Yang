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

//IO��������
#define DHT11_IO_IN()  {IODHT11_GPIO_Port->CRH &= 0xFF0FFFFF;	IODHT11_GPIO_Port->CRH |= 8 << 20;}	// PC13 IN  MODE
#define DHT11_IO_OUT() {IODHT11_GPIO_Port->CRH &= 0xFF0FFFFF;	IODHT11_GPIO_Port->CRH |= 3 << 20;}	// PC13 OUT MODE

////IO��������
#define	DHT11_DQ_OUT PCout(13) //���ݶ˿�	PC13
#define	DHT11_DQ_IN  PCin(13)  //���ݶ˿�	PC13

u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp, u8 *humi); //��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11
u8 DHT11_Read_Data_Float(float *temp,float *humi);

#endif

