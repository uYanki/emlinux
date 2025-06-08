

#include "ssd1306_i2c.h"
#include "i2crw.h"
#include "config.h"

#define PAGE_START_ADDR      0xB0
#define LOWER_SATRT_COL_ADDR 0x00
#define UPPER_SATRT_COL_ADDR 0x10

#define BUFFER_SIZE          (128 * 8)

#define MAX(n1, n2)          ((n1) > (n2) ? (n1) : (n2))
#define MIN(n1, n2)          ((n1) < (n2) ? (n1) : (n2))

//-----------------------------------------------------------------------------
// port

static int fd = 0;

uint8_t SSD1306_Buffer[BUFFER_SIZE] = {0};

void SSD1306_WriteByte(uint8_t Addr, uint8_t Data);
void SSD1306_WriteMem(uint8_t Addr, uint8_t* pData, uint16_t Len);
void SSD1306_WriteCmd(uint8_t cmd);

void SSD1306_I2C_Init(void)
{
    wiringPiSetupSys();
    fd = wiringPiI2CSetupInterface(LINUX_IIC_FILE, OLED_ADDR);
}

void SSD1306_WriteByte(uint8_t Addr, uint8_t Data)
{
    wiringPiI2CWriteReg8(fd, Addr, Data);
}

void SSD1306_WriteMem(uint8_t Addr, uint8_t* pData, uint16_t Len)
{
    I2C_WirteMem(fd, Addr, pData, Len);
}

void SSD1306_WriteCmd(uint8_t cmd)
{
    SSD1306_WriteByte(0x00, cmd);
}

//-----------------------------------------------------------------------------
//

void SSD1306_Init(void)
{
    SSD1306_I2C_Init();

    SSD1306_WriteCmd(0xAE);  // 关闭显示

    SSD1306_WriteCmd(0xA8);  // 设置多路复用比例，默认A8h，63
    SSD1306_WriteCmd(0x3F);

    SSD1306_WriteCmd(0xD3);  // 设置显示偏移，默认D3h，0
    SSD1306_WriteCmd(0x00);

    SSD1306_WriteCmd(0x40);  // 设置显示起始行

    // 水平翻转
#ifdef MIRROR_HORIZONTAL
    SSD1306_WriteCmd(0xA0);
#else
    SSD1306_WriteCmd(0xA1);
#endif  // MIRROR_HORIZONTAL
    // 垂直翻转
#ifdef MIRROR_VERTICAL
    SSD1306_WriteCmd(0xC0);
#else
    SSD1306_WriteCmd(0xC8);
#endif

    SSD1306_WriteCmd(0xDA);  // 设置COM引脚硬件配置，默认DAh，02
    SSD1306_WriteCmd(0x12);

    SSD1306_WriteCmd(0x81);  // 设置对比度控制，默认81h，7Fh
    SSD1306_WriteCmd(0x7F);

    SSD1306_WriteCmd(0xA4);  // 设置显示内容来自于GDDRAM

    SSD1306_WriteCmd(0xA6);  // 设置不反转颜色

    SSD1306_WriteCmd(0xD5);  // 设置晶振频率，默认D5h，80h
    SSD1306_WriteCmd(0x80);

    SSD1306_WriteCmd(0x8D);  // 设置电荷泵稳压器，默认8Dh，14h
    SSD1306_WriteCmd(0x14);

    SSD1306_WriteCmd(0xAF);  // 打开屏幕

    SSD1306_ClearScreen();
    SSD1306_UpdateScreen();
}

void SSD1306_InvertColor(SSD1306_State_e state)
{
    if (state == SSD1306_ENABLE)
    {
        SSD1306_WriteCmd(0xA7);  // 反色显示
    }
    else
    {
        SSD1306_WriteCmd(0xA6);  // 正常显示
    }
}

void SSD1306_ClearScreen(void)
{
    memset(SSD1306_Buffer, 0x00, BUFFER_SIZE);

#ifndef FULL_REFRESH
    SSD1306_UpdateScreen();
#endif
}

void SSD1306_FullScreen(void)
{
    memset(SSD1306_Buffer, 0xFF, BUFFER_SIZE);

#ifndef FULL_REFRESH
    SSD1306_UpdateScreen();
#endif
}

void SSD1306_UpdateScreen(void)
{
    for (uint8_t j = 0; j < 4; j++)
    {
        for (uint8_t i = 0; i < 64 / 8; i++)
        {
            SSD1306_WriteCmd(PAGE_START_ADDR + i);
            SSD1306_WriteCmd(LOWER_SATRT_COL_ADDR + ((32 * j) & 0x0F));
            SSD1306_WriteCmd(UPPER_SATRT_COL_ADDR + (((32 * j) & 0xF0) >> 4));
            SSD1306_WriteMem(0x40, &SSD1306_Buffer[128 * i + j * 32], 32);
        }
    }
}

void SSD1306_TurnOn(SSD1306_State_e state)
{
    if (state == SSD1306_ENABLE)
    {
        SSD1306_WriteCmd(0xAF);
    }
    else
    {
        SSD1306_WriteCmd(0xAE);
    }
}

void SSD1306_PutChar(uint8_t x, uint8_t y, char c, font_t font, SSD1306_Color_e color)
{
    uint32_t byte;

    if (c < 32 || c > 126)
    {
        // unsupported chars
        return;
    }

    if (128 < (x + font.width) || 64 < (y + font.height))
    {
        // out of screen
        return;
    }

    for (uint32_t i = 0; i < font.height; i++)
    {
        byte = font.bitmap[(c - 32) * font.height + i];

        for (uint32_t j = 0; j < font.width; j++)
        {
            SSD1306_DrawPixel(x + j, y + i, ((byte << j) & 0x8000) == 0 ? !color : color);
        }
    }
}

void SSD1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_Color_e color)
{
    if (x >= 128 || y >= 64)
    {
        return;
    }

    uint8_t COM_y = y / 8;
    uint8_t Bit_y = y % 8;

    if (color == White)
    {
        SSD1306_Buffer[128 * COM_y + x] |= (0x01 << Bit_y);
    }
    else
    {
        SSD1306_Buffer[128 * COM_y + x] &= ~(0x01 << Bit_y);
    }

#ifndef FULL_REFRESH
    SSD1306_WriteCmd(0xB0 + COM_y);
    SSD1306_WriteCmd(0x00 + x % 16);
    SSD1306_WriteCmd(0x10 + x / 16);
    SSD1306_WriteByte(0x40, SSD1306_Buffer[128 * COM_y + x]);
#endif
}

void SSD1306_DrawBitMap(uint8_t x, uint8_t y, const uint8_t* bitmap, uint8_t width, uint8_t height, SSD1306_Color_e color)
{
    uint8_t byte_width = (width + 7) / 8;
    uint8_t byte       = 0;

    for (int16_t j = 0; j < height; j++, y++)
    {
        for (int16_t i = 0; i < width; i++)
        {
            if (i & 7)
            {
                byte <<= 1;
            }
            else
            {
                byte = bitmap[j * byte_width + i / 8];
            }
            if (byte & 0x80)
            {
                SSD1306_DrawPixel(x + i, y, color);
            }
        }
    }
}

void SSD1306_PutString(uint8_t x, uint8_t y, char* c, font_t font, SSD1306_Color_e color)
{
    uint16_t len = strlen(c);
    for (uint16_t i = 0; i < len; i++)
    {
        SSD1306_PutChar(x + i * font.width, y, c[i], font, color);
    }
}

void SSD1306_SetContrast(uint8_t val)
{
    SSD1306_WriteCmd(0x81);
    SSD1306_WriteCmd(val);
}

void SSD1306_DrawLine(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y, SSD1306_Color_e color)
{
    if (start_x == end_x)  // k=0
    {
        uint8_t sy = start_y > end_y ? end_y : start_y;
        uint8_t ey = start_y < end_y ? end_y : start_y;

        for (uint8_t i = sy; i <= ey; i++)
        {
            SSD1306_DrawPixel(start_x, i, color);
        }
    }
    else
    {
        double k = 1.0 * (start_y - end_y) / (start_x - end_x);
        double b = start_y - k * start_x;

        uint8_t sx = start_x > end_x ? end_x : start_x;
        uint8_t ex = start_x < end_x ? end_x : start_x;

        for (uint8_t i = sx; i <= ex; i++)
        {
            SSD1306_DrawPixel(i, (uint8_t)(k * i + b), color);
        }
    }
}

void SSD1306_DrawRectangle1(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y, SSD1306_Color_e color)
{
    SSD1306_DrawLine(start_x, start_y, end_x, start_y, color);
    SSD1306_DrawLine(start_x, start_y, start_x, end_y, color);
    SSD1306_DrawLine(start_x, end_y, end_x, end_y, color);
    SSD1306_DrawLine(end_x, start_y, end_x, end_y, color);
}

void SSD1306_DrawRectangle2(uint8_t start_x, uint8_t start_y, uint8_t width, uint8_t height, SSD1306_Color_e color)
{
    SSD1306_DrawRectangle1(start_x, start_y, start_x + width, start_y + height, color);
}

void SSD1306_FillRectXY(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y, SSD1306_Color_e color)
{
    uint8_t sx = MIN(start_x, end_x);
    uint8_t sy = MIN(start_y, end_y);
    uint8_t ex = MAX(start_x, end_x);
    uint8_t ey = MAX(start_y, end_y);

    for (uint8_t i = sx; i <= ex; i++)
    {
        for (uint8_t j = sy; j <= ey; j++)
        {
            SSD1306_DrawPixel(i, j, color);
        }
    }
}

void SSD1306_FillRectWH(uint8_t start_x, uint8_t start_y, uint8_t width, uint8_t height, SSD1306_Color_e color)
{
    SSD1306_FillRectXY(start_x, start_y, start_x + width, start_y + height, color);
}
