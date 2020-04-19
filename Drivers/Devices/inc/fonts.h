#ifndef FONTS_H
#define FONTS_H 120

#include "main.h"
#include <string.h>

/**
 * @brief  LCD库中使用的字体结构
 */
typedef struct
{
  uint8_t FontWidth;    /*!< Font width in pixels */
  uint8_t FontHeight;   /*!< Font height in pixels */
  uint8_t StrSize;   /*!< 一个字符所占多少个字节 */
  const uint8_t *data; /*!< Pointer to data font data array */
} FontDef_t;


typedef struct
{
  char name[3];  // 结构体的第一个成员name[3] 用来存放汉字的区码和位码以及一个字符串结束符 ‘\0’ ，这里的 ‘\0’ 并不是必要的。
  char dat[32];  // 结构体的第二个成员dat[32] 用来存放汉字点阵数据，这里采用的是16*16的汉字，16*16/8=32，故单个汉字的点阵数据为32字节。
} chinese;

/** 
 * @brief  管柱长度和高度
 */
typedef struct
{
  uint16_t Length; /*!< String length in units of pixels */
  uint16_t Height; /*!< String height in units of pixels */
} FONTS_SIZE_t;


/**
 * @brief  7 x 10 像素字号结构
 */
extern FontDef_t Font_6x8;

/**
 * @brief  8 x 16 像素字号结构
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
 * @brief  根据使用的字符串和字体，以像素为单位计算字符串长度和高度
 * @param  *str: String to be checked for length and height
 * @param  *SizeStruct: Pointer to empty @ref FONTS_SIZE_t structure where informations will be saved
 * @param  *Font: Pointer to @ref FontDef_t font used for calculations
 * @retval Pointer to string used for length and height
 */
char *FONTS_GetStringSize(char *str, FONTS_SIZE_t *SizeStruct, FontDef_t *Font);

#endif
