// i2cdetect -y 2
// g++ -o app *.cpp -lwiringPi -lpthread

#include <stdio.h>
#include "bmp280.h"
#include "wiringPi.h"

int main(void)
{
    bmp280 bmp;
    bmp.init(0x77);

    while (1)
    {
        float p = (float)bmp.read_pressure();
        float t = (float)bmp.read_temperature();

        // Pressure (Pa), Temperature (°C)
        printf("\r %10f Pa, %10f °C", p, t);
        fflush(stdout);

        delay(300);
    }
}