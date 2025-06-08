#include "st7735.h"

extern const unsigned char gImage_test_image[32768];

int main(void)
{
    ST7735_Init();
    ST7735_DrawImage(0, 0, 128, 128, gImage_test_image);
}
