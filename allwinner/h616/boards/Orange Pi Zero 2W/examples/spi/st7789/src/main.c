#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include "st7789.h"
#include "bmp_decoder.h"

extern const unsigned char gImage_ys_320_172[];

int main()
{
    ST7789_Init();

    // backlight control
    ST7789_BLKCtrl(HIGH);

    // basic test
    ST7789_Test();
    sleep(1);

    // display imgae
    ST7789_Fill_Color(BLACK);
    ST7789_DrawImage(0, 0, 320, 172, (uint16_t*)gImage_ys_320_172);
    sleep(2);

    // BMP encoder test
    ST7789_Fill_Color(BLACK);
    DecodeRet ret = -1;
    ShowBMPImage("./ys_logo.bmp", 0, 0, &ret);
    printf("%d\n", ret);
}
