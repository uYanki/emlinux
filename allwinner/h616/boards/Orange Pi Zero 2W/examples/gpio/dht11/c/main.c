
// gcc -Wall -o app *.c -lwiringPi -lpthread

#include "dht11.h"
#include <stdio.h>

int main(void)
{
    uint8_t times = 10;
    uint8_t temperature;
    uint8_t humidity;

    wiringPiSetup();

    while (times--)
    {
        if (dht11_read_data(&temperature, &humidity) == true)
        {
            printf("temp: %d humi: %d\n", temperature, humidity);
            fflush(stdout);
            delay(1000);
        }
        else
        {
            // 到这部可能是县受到干扰, 或者没上拉造成的
            printf("fail to read dht11\n");
        }
    }

    return 0;
}
