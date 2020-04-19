#include "oled12864.h"

// ����ֵ
#define ABS(x) ((x) > 0 ? (x) : -(x))

/* SSD1306 data buffer */
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// ˽�нṹ��
typedef struct
{
  uint16_t CurrentX;
  uint16_t CurrentY;
  uint8_t Inverted;
  uint8_t Initialized;
} SSD1306_t;

// SSD1306 ˽�б���
static SSD1306_t SSD1306;

// д����
uint8_t SSD1306_Write(uint8_t data, Write_Model model)
{
  HAL_StatusTypeDef dmaxferstatus;
  uint8_t buf[2] = {0};
  buf[0] = model;
  buf[1] = data;
  dmaxferstatus = HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR, buf, 2, 1);
  if (dmaxferstatus == HAL_OK)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

// д����
uint8_t SSD306_Write_DATA(uint8_t *data, uint16_t size)
{
  HAL_StatusTypeDef dmaxferstatus;
  uint8_t buf[size + 1];
  uint16_t i;
  buf[0] = 0x40;
  for (i = 0; i < size; i++)
  {
    buf[i + 1] = data[i];
  }
  dmaxferstatus = HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR, buf, size + 1, 1);
  if (dmaxferstatus == HAL_OK)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

/** 
 * @brief  �����Ļ
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  Color: Color to be used for screen fill. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_Fill(SSD1306_COLOR_t color)
{
  // �����ڴ�
  memset(SSD1306_Buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SSD1306_Buffer));
}

/** 
 * @brief  ������ʾ
 * @note   This function must be called each time you do some changes to LCD, to update buffer from RAM to LCD
 * @param  None
 * @retval None
 */
void SSD1306_UpdateScreen(void)
{
  uint8_t m;
  for (m = 0; m < 8; m++)
  {
    SSD1306_Write(0xB0 + m, SSD1306_CMD); //����ҳ��ַ��0~7��
    SSD1306_Write(0x00, SSD1306_CMD);     //������ʾλ�á��е͵�ַ
    SSD1306_Write(0x10, SSD1306_CMD);     //������ʾλ�á��иߵ�ַ

    // д������
    SSD306_Write_DATA(&SSD1306_Buffer[SSD1306_WIDTH * m], SSD1306_WIDTH);
  }
}

/**
 * @brief  �л��ڲ�RAM�е����ط�ת
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  None
 * @retval None
 */
void SSD1306_ToggleInvert(void)
{
  uint16_t i;
  /* �л���ת */
  SSD1306.Inverted = !SSD1306.Inverted;
  /* ִ���ڴ��л� */
  for (i = 0; i < sizeof(SSD1306_Buffer); i++)
  {
    SSD1306_Buffer[i] = ~SSD1306_Buffer[i];
  }
}

/**
 * @brief  ������λ�û�������
 * @note   @ref SSD1306_UpdateScreen() must called after that in order to see updated LCD screen
 * @param  x: X location. This parameter can be a value between 0 and SSD1306_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SSD1306_HEIGHT - 1
 * @param  color: Color to be used for screen fill. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color)
{
  if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
  {
    // error
    return;
  }
  /* ��������Ƿ�ת*/
  if (SSD1306.Inverted)
  {
    color = (SSD1306_COLOR_t)!color;
  }
  /* ������ɫ */
  if (color == SSD1306_COLOR_WHITE)
  {
    SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
  }
  else
  {
    SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
  }
}

/**
 * @brief  �����ָ��ָ�����ַ���λ��
 * @param  x: X location. This parameter can be a value between 0 and SSD1306_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SSD1306_HEIGHT - 1
 * @retval None
 */
void SSD1306_GotoXY(uint16_t x, uint16_t y)
{
  SSD1306.CurrentX = x;
  SSD1306.CurrentY = y;
}

/**
 * @brief  �ַ�д���ڴ�
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  ch: Character to be written
 * @param  *Font: Pointer to @ref FontDef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @param  B: �����Ƿ�Ӵ�
 * @retval Character written
 */
char SSD1306_Putc(char ch, FontDef_t *Font, SSD1306_COLOR_t color, FontWeight B)
{
  uint16_t n, i, j, data, index = 0, singleDataSize = 8;

  /* �����ÿռ� ��ⷶΧ */
  if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font->FontWidth) || SSD1306_HEIGHT <= (SSD1306.CurrentY + Font->FontHeight))
  {
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = SSD1306.CurrentY + 2;
    /* Error */
    // return 0;
  }

  uint8_t num = (Font->StrSize + singleDataSize - 1) / singleDataSize;
  for (n = 0; n < num; n++)
  {
    for (i = 0; i < Font->FontWidth; i++)
    {
      index = (ch - 32) * Font->StrSize + i + (n * singleDataSize);
      data = Font->data[index];
      if (B == bold)
        if (index != 0 && i != 0)
          data = Font->data[index - 1] | data;
      for (j = 0; j < singleDataSize; j++)
      {
        if ((data >> j) & 0x01)
        {
          SSD1306_DrawPixel(SSD1306.CurrentX + i, (SSD1306.CurrentY + j + (n * singleDataSize)), (SSD1306_COLOR_t)color);
        }
        else
        {
          SSD1306_DrawPixel(SSD1306.CurrentX + i, (SSD1306.CurrentY + j + (n * singleDataSize)), (SSD1306_COLOR_t)!color);
        }
      }
    }
  }
  // /* Increase pointer */
  SSD1306.CurrentX += Font->FontWidth;
  /* Return character written */
  return ch;
}

/**
 * @brief  �ַ���д���ڴ�
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  *str: String to be written
 * @param  *Font: Pointer to @ref FontDef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval Zero on success or character value when function failed
 */
char SSD1306_Puts(char *str, FontDef_t *Font, SSD1306_COLOR_t color, FontWeight B)
{
  /* Write characters */
  while (*str)
  {
    /* Write character by character */
    if (SSD1306_Putc(*str, Font, color, B) != *str)
    {
      /* Return error */
      return *str;
    }
    /* Increase string pointer */
    str++;
  }
  /* Everything OK, zero should be returned */
  return *str;
}

/**
 * @brief  ����
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x0: Line X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y0: Line Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x1: Line X end point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: Line Y end point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c)
{
  int16_t dx, dy, sx, sy, err, e2, i, tmp;

  /* Check for overflow */
  if (x0 >= SSD1306_WIDTH)
  {
    x0 = SSD1306_WIDTH - 1;
  }
  if (x1 >= SSD1306_WIDTH)
  {
    x1 = SSD1306_WIDTH - 1;
  }
  if (y0 >= SSD1306_HEIGHT)
  {
    y0 = SSD1306_HEIGHT - 1;
  }
  if (y1 >= SSD1306_HEIGHT)
  {
    y1 = SSD1306_HEIGHT - 1;
  }

  dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
  dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
  sx = (x0 < x1) ? 1 : -1;
  sy = (y0 < y1) ? 1 : -1;
  err = ((dx > dy) ? dx : -dy) / 2;

  if (dx == 0)
  {
    if (y1 < y0)
    {
      tmp = y1;
      y1 = y0;
      y0 = tmp;
    }

    if (x1 < x0)
    {
      tmp = x1;
      x1 = x0;
      x0 = tmp;
    }

    /* Vertical line */
    for (i = y0; i <= y1; i++)
    {
      SSD1306_DrawPixel(x0, i, c);
    }

    /* Return from function */
    return;
  }

  if (dy == 0)
  {
    if (y1 < y0)
    {
      tmp = y1;
      y1 = y0;
      y0 = tmp;
    }

    if (x1 < x0)
    {
      tmp = x1;
      x1 = x0;
      x0 = tmp;
    }

    /* Horizontal line */
    for (i = x0; i <= x1; i++)
    {
      SSD1306_DrawPixel(i, y0, c);
    }

    /* Return from function */
    return;
  }

  while (1)
  {
    SSD1306_DrawPixel(x0, y0, c);
    if (x0 == x1 && y0 == y1)
    {
      break;
    }
    e2 = err;
    if (e2 > -dx)
    {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dy)
    {
      err += dx;
      y0 += sy;
    }
  }
}

void SSD1306_DrawLine2(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c)
{
  int x, y, dx, dy, d;
  y = y0;
  dx = x1 - x0;
  dy = y1 - y0;
  d = 2 * dy - dx;
  for (x = x0; x <= x1; x++)
  {
    SSD1306_DrawPixel(x, y, c);
    if (d < 0)
    {
      d += 2 * dy;
    }
    else
    {
      y++;
      d += 2 * dy - 2 * dx;
    }
  }
}

/**
 * @brief  ������
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c)
{
  /* Check input parameters */
  if (
      x >= SSD1306_WIDTH ||
      y >= SSD1306_HEIGHT)
  {
    /* Return error */
    return;
  }

  /* Check width and height */
  if ((x + w) >= SSD1306_WIDTH)
  {
    w = SSD1306_WIDTH - x;
  }
  if ((y + h) >= SSD1306_HEIGHT)
  {
    h = SSD1306_HEIGHT - y;
  }

  /* Draw 4 lines */
  SSD1306_DrawLine(x, y, x + w, y, c);         /* Top line */
  SSD1306_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
  SSD1306_DrawLine(x, y, x, y + h, c);         /* Left line */
  SSD1306_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}

/**
 * @brief  ���ľ���
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c)
{
  uint8_t i;
  /* Check input parameters */
  if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
  {
    /* Return error */
    return;
  }
  /* Check width and height */
  if ((x + w) >= SSD1306_WIDTH)
  {
    w = SSD1306_WIDTH - x;
  }
  if ((y + h) >= SSD1306_HEIGHT)
  {
    h = SSD1306_HEIGHT - y;
  }
  /* Draw lines */
  for (i = 0; i <= h; i++)
  {
    /* Draw lines */
    SSD1306_DrawLine(x, y + i, x + w, y + i, c);
  }
}

/**
 * @brief  ��������
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x1: First coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: First coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x2: Second coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y2: Second coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x3: Third coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y3: Third coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color)
{
  /* Draw lines */
  SSD1306_DrawLine(x1, y1, x2, y2, color);
  SSD1306_DrawLine(x2, y2, x3, y3, color);
  SSD1306_DrawLine(x3, y3, x1, y1, color);
}

/**
 * @brief  �����������
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x1: First coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: First coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x2: Second coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y2: Second coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x3: Third coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y3: Third coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
          yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
          curpixel = 0;

  deltax = ABS(x2 - x1);
  deltay = ABS(y2 - y1);
  x = x1;
  y = y1;

  if (x2 >= x1)
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1)
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)
  {
    xinc1 = 0;
    yinc2 = 0;
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;
  }
  else
  {
    xinc2 = 0;
    yinc1 = 0;
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    SSD1306_DrawLine(x, y, x3, y3, color);

    num += numadd;
    if (num >= den)
    {
      num -= den;
      x += xinc1;
      y += yinc1;
    }
    x += xinc2;
    y += yinc2;
  }
}

/**
 * @brief  ��Բ
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  SSD1306_DrawPixel(x0, y0 + r, c);
  SSD1306_DrawPixel(x0, y0 - r, c);
  SSD1306_DrawPixel(x0 + r, y0, c);
  SSD1306_DrawPixel(x0 - r, y0, c);

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    SSD1306_DrawPixel(x0 + x, y0 + y, c);
    SSD1306_DrawPixel(x0 - x, y0 + y, c);
    SSD1306_DrawPixel(x0 + x, y0 - y, c);
    SSD1306_DrawPixel(x0 - x, y0 - y, c);

    SSD1306_DrawPixel(x0 + y, y0 + x, c);
    SSD1306_DrawPixel(x0 - y, y0 + x, c);
    SSD1306_DrawPixel(x0 + y, y0 - x, c);
    SSD1306_DrawPixel(x0 - y, y0 - x, c);
  }
}

/**
 * @brief  ������Բ
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  SSD1306_DrawPixel(x0, y0 + r, c);
  SSD1306_DrawPixel(x0, y0 - r, c);
  SSD1306_DrawPixel(x0 + r, y0, c);
  SSD1306_DrawPixel(x0 - r, y0, c);
  SSD1306_DrawLine(x0 - r, y0, x0 + r, y0, c);

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    SSD1306_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
    SSD1306_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

    SSD1306_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
    SSD1306_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
  }
}

// ��ʾһ�������ַ� 
void OLED_Show_16x16HZ(uint8_t *str, SSD1306_COLOR_t color, FontWeight B)
{
  uint8_t t = 0, data;
  uint16_t i, j;
  for (i = 0; i < sizeof(Hzk16x16) / 35; i++)
	{
		if (Hzk16x16[i].name[0] == str[0] && Hzk16x16[i].name[1] == str[1]) //�ԱȺ�������λ��
		{
			for (t = 0; t < 16; t++) //��д�����ϰ벿������
			{
        data = Hzk16x16[i].dat[t];
        if (B == bold && t != 0)
          data = Hzk16x16[i].dat[t-1] | data;
        for (j = 0; j < 8; j++)
        {
          if ((data >> j) & 0x01)
          {
            SSD1306_DrawPixel(SSD1306.CurrentX + t, (SSD1306.CurrentY + j), (SSD1306_COLOR_t)color);
          }
          else
          {
            SSD1306_DrawPixel(SSD1306.CurrentX + t, (SSD1306.CurrentY + j), (SSD1306_COLOR_t)!color);
          }
        }
			}
			for (t = 0; t < 16; t++) //��д�����°벿������
			{
        data = Hzk16x16[i].dat[t + 16];
        if (B == bold && t != 0)
          data = Hzk16x16[i].dat[t + 16 - 1] | data;
        for (j = 0; j < 8; j++)
        {
          if ((data >> j) & 0x01)
          {
            SSD1306_DrawPixel(SSD1306.CurrentX + t, (SSD1306.CurrentY + j + 8), (SSD1306_COLOR_t)color);
          }
          else
          {
            SSD1306_DrawPixel(SSD1306.CurrentX + t, (SSD1306.CurrentY + j + 8), (SSD1306_COLOR_t)!color);
          }
        }
			}
		}
	}
  SSD1306.CurrentX += 16;
}

void OLED_ShowText(uint8_t *str, SSD1306_COLOR_t color, FontWeight B)
{
  uint8_t tempstr[3] = {'\0'};
  while (*str != '\0')
  {
    if (*str & 0x80)
		{
			tempstr[0] = *str++;
			tempstr[1] = *str++;
			OLED_Show_16x16HZ(tempstr, color, B);
			// SSD1306.CurrentX += 16;
			if (SSD1306.CurrentX >= 112)
			{
				SSD1306.CurrentY++;
				SSD1306.CurrentY++;
				SSD1306.CurrentX = 0;
			} //�޸ĵ�ַ
		}
		else
		{
      SSD1306_Putc(*str++, &Font_8x16, color, B);
			// SSD1306.CurrentX += 8;
			if (SSD1306.CurrentX >= 112)
			{
				SSD1306.CurrentY++;
				SSD1306.CurrentY++;
				SSD1306.CurrentX = 0;
			} //�޸ĵ�ַ
		}
  }
}

void SSD1306_ON(void)
{
  SSD1306_Write(0x8D, SSD1306_CMD);
  SSD1306_Write(0x14, SSD1306_CMD);
  SSD1306_Write(0xAF, SSD1306_CMD);
}

void SSD1306_OFF(void)
{
  SSD1306_Write(0x8D, SSD1306_CMD);
  SSD1306_Write(0x10, SSD1306_CMD);
  SSD1306_Write(0xAE, SSD1306_CMD);
}

/**
 * @brief  ������ʾĳһҳ
 * @param  start: ��ʼҳ 0 - 7
 * @param  end: ����ҳ 0 - 7
 * @param  dirct: ����ö�٣�left����right����
 * @retval None
 */
void OLED_Scroll_Display(uint8_t start, uint8_t end, enum DIRECTION dirct)
{
  if (start > 7 || end > 7)
    return;                  // ҳ�볬��
  SSD1306_Write(0x2E, SSD1306_CMD);  // �رչ���
  SSD1306_Write(dirct, SSD1306_CMD); // ����/�ҹ���
  SSD1306_Write(0x00, SSD1306_CMD);  // ������
  SSD1306_Write(start, SSD1306_CMD); // ��ʼҳ
  SSD1306_Write(0x0F, SSD1306_CMD);  // ���������0=��������ֵԽ�󣬹���Խ��
  SSD1306_Write(end, SSD1306_CMD);   // ����ҳ
  SSD1306_Write(0x00, SSD1306_CMD);  // ��ָ��
  SSD1306_Write(0xFF, SSD1306_CMD);  // ��ָ���������ָ���Ȼ������Ч
  SSD1306_Write(0x2F, SSD1306_CMD);  // ��������
}

uint8_t SSD1306_Init(void)
{

  Delay_ms(500);

  SSD1306_Write(0xAE, SSD1306_CMD); // ����ʾ
  SSD1306_Write(0x20, SSD1306_CMD); // �����ڴ�Ѱַģʽ
  SSD1306_Write(0x00, SSD1306_CMD); // 00��ˮƽѰַģʽ;01����ֱѰַģʽ;10��ҳ��Ѱַģʽ(��λ);11����Ч
  SSD1306_Write(0xB0, SSD1306_CMD); // ����ҳ��Ѱַģʽ��ҳ�濪ʼ��ַ��0-7
  SSD1306_Write(0xC8, SSD1306_CMD); // ����COM���ɨ�跽��
  SSD1306_Write(0x00, SSD1306_CMD); // ���õ��е�ַ
  SSD1306_Write(0x10, SSD1306_CMD); // ���ø��е�ַ
  SSD1306_Write(0x40, SSD1306_CMD); // ������ʼ�е�ַ
  SSD1306_Write(0x81, SSD1306_CMD); // ���öԱȶȿ��ƼĴ���
  SSD1306_Write(0x00, SSD1306_CMD);

  SSD1306_Write(0xA1, SSD1306_CMD); // ���ö���ӳ��0��127
  SSD1306_Write(0xA6, SSD1306_CMD); // ����������ʾ
  SSD1306_Write(0xA8, SSD1306_CMD); // ���ö�·���ñ�(1��64)
  SSD1306_Write(0x3F, SSD1306_CMD);

  SSD1306_Write(0xA4, SSD1306_CMD); // 0xa4���������RAM���ݺ���;0xa5���������RAM����
  SSD1306_Write(0xD3, SSD1306_CMD); // ������ʾ����
  SSD1306_Write(0x00, SSD1306_CMD); // ����
  SSD1306_Write(0xD5, SSD1306_CMD); // ������ʾʱ�ӷ�Ƶ��/��Ƶ��
  SSD1306_Write(0xF0, SSD1306_CMD); // �����ֱ���
  SSD1306_Write(0xD9, SSD1306_CMD); // ����pre-chargeʱ��
  SSD1306_Write(0x22, SSD1306_CMD);

  SSD1306_Write(0xDA, SSD1306_CMD); // ����com����Ӳ������
  SSD1306_Write(0x12, SSD1306_CMD);

  SSD1306_Write(0xDB, SSD1306_CMD); // vcomh����
  SSD1306_Write(0x20, SSD1306_CMD); // 0x20,0.77xVcc
  SSD1306_Write(0x8D, SSD1306_CMD); // ����ֱ��-ֱ������
  SSD1306_Write(0x14, SSD1306_CMD);

  SSD1306_Write(0xAF, SSD1306_CMD); // ��SSD1306���

  SSD1306_Fill(SSD1306_COLOR_BLACK); // ����
  SSD1306_UpdateScreen();            // ������ʾ

  // ��ʼ��Ĭ��λ��
  SSD1306.CurrentX = 0;
  SSD1306.CurrentY = 0;

  SSD1306.Initialized = 1;

  return 1;
}
