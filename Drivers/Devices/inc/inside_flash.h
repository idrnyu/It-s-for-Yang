#ifndef __INSIDE_FLASH_H
#define __INSIDE_FLASH_H

#include "main.h"
#include "usart.h"
#include "delay.h"

HAL_StatusTypeDef writeFlash(uint32_t addr, uint32_t writeFlashData);
uint32_t readFlash(uint32_t addr);

#endif

