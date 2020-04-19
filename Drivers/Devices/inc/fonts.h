#ifndef FONTS_H
#define FONTS_H 120

#include "main.h"
#include <string.h>

/**
 * @brief  LCD����ʹ�õ�����ṹ
 */
typedef struct
{
  uint8_t FontWidth;    /*!< Font width in pixels */
  uint8_t FontHeight;   /*!< Font height in pixels */
  uint8_t StrSize;   /*!< һ���ַ���ռ���ٸ��ֽ� */
  const uint8_t *data; /*!< Pointer to data font data array */
} FontDef_t;


typedef struct
{
  char name[3];  // �ṹ��ĵ�һ����Աname[3] ������ź��ֵ������λ���Լ�һ���ַ��������� ��\0�� ������� ��\0�� �����Ǳ�Ҫ�ġ�
  char dat[32];  // �ṹ��ĵڶ�����Աdat[32] ������ź��ֵ������ݣ�������õ���16*16�ĺ��֣�16*16/8=32���ʵ������ֵĵ�������Ϊ32�ֽڡ�
} chinese;

/** 
 * @brief  �������Ⱥ͸߶�
 */
typedef struct
{
  uint16_t Length; /*!< String length in units of pixels */
  uint16_t Height; /*!< String height in units of pixels */
} FONTS_SIZE_t;


/**
 * @brief  7 x 10 �����ֺŽṹ
 */
extern FontDef_t Font_6x8;

/**
 * @brief  8 x 16 �����ֺŽṹ
 */
extern FontDef_t Font_8x16;


extern chinese Hzk[26];


/**
 * @}
 */

/**
 * @defgroup FONTS_Functions
 * @brief    Library functions
 * @{
 */

/**
 * @brief  ����ʹ�õ��ַ��������壬������Ϊ��λ�����ַ������Ⱥ͸߶�
 * @param  *str: String to be checked for length and height
 * @param  *SizeStruct: Pointer to empty @ref FONTS_SIZE_t structure where informations will be saved
 * @param  *Font: Pointer to @ref FontDef_t font used for calculations
 * @retval Pointer to string used for length and height
 */
char *FONTS_GetStringSize(char *str, FONTS_SIZE_t *SizeStruct, FontDef_t *Font);

#endif
