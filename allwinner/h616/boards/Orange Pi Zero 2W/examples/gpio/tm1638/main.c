
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <wiringPi.h>
#include "tm1638.h"

int main(int argc, char* argv[])
{
    tm1638_t t;

    if (wiringPiSetup() == -1)
    {
        printf("wiringPiSetup Error\n");
        exit(1);
    }

    tm1638_init(&t, 23 /* data */, 24 /* clock */, 25 /* strobe */);

    while (1)
    {
        time_t     now = time(NULL);
        struct tm* tm  = localtime(&now);

        char text[10];
        snprintf(text, 9, "%02d %02d %02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
        tm1638_set_7seg_text(&t, text, 0x00);
        tm1638_set_8leds(&t, tm1638_read_8buttons(&t));
        delay(100);
    }

    return 0;
}
