// g++ -o app *.cpp -lwiringPi -lpthread

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>

#include "tcs34725.h"

using namespace std;

int main()
{
    TCS34725 tcs = TCS34725(_700MS, _1X);

    if (tcs.isIdentify())
    {
        cout << "Found sensor" << endl;
    }
    else
    {
        cout << "No TCS34725 found ... check your connections" << endl;
        exit(-1);
    }

    while (1)
    {
        tcs.getRawData();

#if 0
        uint16_t r     = tcs.getRed();
        uint16_t g     = tcs.getGreen();
        uint16_t b     = tcs.getBlue();
        uint16_t c     = tcs.getClear();
        uint16_t crtmp = tcs.calculateColorTemperature();
        uint16_t lux   = tcs.calculateLux();
        printf("\rColor Temp: %5d K, Lux: %5d, R: %5d, G: %5d, B: %5d, C: %5d,", crtmp, lux, r, g, b, c);
#else
        uint32_t rgb888 = tcs.getRGB888();
        printf("\rR: %5d, G: %5d, B: %5d,", rgb888 >> 16, (rgb888 >> 8) & 0xFF, rgb888 & 0xFF);
#endif

        fflush(stdout);

        delay(100);
    }
}
