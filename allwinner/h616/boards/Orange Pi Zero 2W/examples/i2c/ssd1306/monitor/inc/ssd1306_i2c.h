
#ifndef __SSD1306_I2C_H__
#define __SSD1306_I2C_H__

#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "font.h"

#define OLED_ADDR 0x3C
#define FULL_REFRESH
// #define MIRROR_VERTICAL
// #define MIRROR_HORIZONTAL

typedef enum {
    White = 1,
    Black = !White,
} SSD1306_Color_e;

typedef enum {
    SSD1306_ENABLE  = 0,
    SSD1306_DISABLE = !SSD1306_ENABLE,
} SSD1306_State_e;

void SSD1306_Init(void);

// Graphic Function
void SSD1306_FillRectXY(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y, SSD1306_Color_e color);
void SSD1306_FillRectWH(uint8_t start_x, uint8_t start_y, uint8_t width, uint8_t height, SSD1306_Color_e color);
void SSD1306_DrawRectangle1(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y, SSD1306_Color_e color);
void SSD1306_DrawRectangle2(uint8_t start_x, uint8_t start_y, uint8_t width, uint8_t height, SSD1306_Color_e color);
void SSD1306_DrawLine(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y, SSD1306_Color_e color);
void SSD1306_PutString(uint8_t x, uint8_t y, char* c, font_t font, SSD1306_Color_e color);
void SSD1306_PutChar(uint8_t x, uint8_t y, char c, font_t font, SSD1306_Color_e color);
void SSD1306_DrawBitMap(uint8_t x, uint8_t y, const uint8_t* bitmap, uint8_t width, uint8_t height, SSD1306_Color_e color);
void SSD1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_Color_e color);
void SSD1306_ClearScreen(void);
void SSD1306_FullScreen(void);
void SSD1306_UpdateScreen(void);

// Display Function
void SSD1306_SetContrast(uint8_t val);
void SSD1306_TurnOn(SSD1306_State_e state);
void SSD1306_InvertColor(SSD1306_State_e state);

#endif
