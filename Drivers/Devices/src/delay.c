#include "delay.h"

static uint8_t D_us = 0;	// ��̬����   ΢��ϵ��
static uint16_t D_ms = 0; // ��̬����   ����ϵ��

void Delay_Init(void)
{
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);

	D_us = SystemCoreClock / 8000000;
	D_ms = (uint16_t)D_us * 1000;
}

void Delay_us(uint32_t nus)
{
	uint32_t temp;
	SysTick->CTRL = 0x00;				// �ر�SysTick��ʱ��
	SysTick->LOAD = nus * D_us; // ��װ��ֵ
	SysTick->VAL = 0x00;				// ��յ�ǰֵ������
	SysTick->CTRL |= 0x01;			// ����SysTick��ʱ��
	do
	{
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16))); // �ȴ���ʱ����
	SysTick->CTRL = 0x00;														// �ر�SysTick��ʱ��
	SysTick->VAL = 0x00;														// ��ռ�����
}

void Delay_ms(uint32_t nms)
{
	uint32_t temp;
	SysTick->CTRL = 0x00;
	SysTick->LOAD = nms * D_ms;
	SysTick->VAL = 0x00;
	SysTick->CTRL |= 0x01;
	do
	{
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16)));
	SysTick->CTRL = 0x00;
	SysTick->VAL = 0x00;
}
