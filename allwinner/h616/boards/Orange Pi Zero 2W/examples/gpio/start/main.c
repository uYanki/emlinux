// gpio readall
// gcc -Wall -o app main.c -lwiringPi -lpthread
// sudo ./app

#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <stdint.h>
#include <softPwm.h>

#define PWM     1
#define LED     2

#define UNIT_MS 1000

int main(void)
{
    wiringPiSetup();

    pinMode(LED, OUTPUT);
    softPwmCreate(PWM, 0, 100);

    uint16_t duty = 0;

    while (1)
    {
        digitalWrite(LED, HIGH);
        usleep(10 * UNIT_MS);
        digitalWrite(LED, LOW);
        usleep(10 * UNIT_MS);

        softPwmWrite(PWM, duty++);
        if (duty == 100) { duty = 0; }
    }
}
