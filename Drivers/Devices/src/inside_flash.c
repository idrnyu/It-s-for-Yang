#include "inside_flash.h"

//FLASHд�����ݲ���
HAL_StatusTypeDef writeFlash(uint32_t addr, uint32_t writeFlashData)
{
  uint32_t PageError = 0;
  FLASH_EraseInitTypeDef FlashSet;
  HAL_StatusTypeDef status;

  // 1������FLASH  ��ʼ��FLASH_EraseInitTypeDef
  FlashSet.TypeErase = FLASH_TYPEERASE_PAGES;
	FlashSet.PageAddress = addr;
	FlashSet.NbPages = 1;

  // 2������FLASH
  HAL_FLASH_Unlock();
  // ���ò�������
  status = HAL_FLASHEx_Erase(&FlashSet, &PageError);
  if(status != HAL_OK)
	{
		printf("erase fail, PageError = %d\r\n", PageError);
    return status;
	}
	printf("erase success\r\n");
  //3����FLASH��д
	status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, writeFlashData);
  if(status != HAL_OK)
  {
    printf("program fail, PageError = %d\r\n", PageError);
  }
  printf("program success\r\n");
  //4����סFLASH
  HAL_FLASH_Lock();
  return status;
}

//FLASH��ȡ���ݲ���
uint32_t readFlash(uint32_t addr)
{
  uint32_t temp = *(__IO uint32_t*)(addr);
  printf("addr:0x%x, data:0x%x\r\n", addr, temp);
  return temp;
}


