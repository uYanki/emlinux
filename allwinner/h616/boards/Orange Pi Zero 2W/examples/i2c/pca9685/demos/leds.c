

#include "pca9685.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <stdio.h>
#include <stdlib.h>

#define I2CDEV   "/dev/i2c-2"
#define PIN_BASE 300
#define MAX_PWM  4096
#define HERTZ    50

int main(void)
{
    printf("PCA9685 LED example\n");

    // Calling wiringPi setup first.
    wiringPiSetup();

    int fd = pca9685Setup(I2CDEV, PIN_BASE, 0x40, HERTZ);
    printf(I2CDEV "\n");
    if (fd < 0)
    {
        printf("Error in setup\n");
        return fd;
    }

    pca9685PWMReset(fd);

    int i, j;
    int active = 1;

    while (active)
    {
        for (j = 0; j < 5; j++)
        {
            for (i = 0; i < MAX_PWM; i += 32)
            {
                pwmWrite(PIN_BASE + 16, i);
                delay(4);
            }

            for (i = 0; i < MAX_PWM; i += 32)
            {
                pwmWrite(PIN_BASE + 16, MAX_PWM - i);
                delay(4);
            }
        }

        pwmWrite(PIN_BASE + 16, 0);
        delay(500);

        for (j = 0; j < 5; j++)
        {
            for (i = 0; i < 16; i++)
            {
                pwmWrite(PIN_BASE + i, MAX_PWM);
                delay(20);
            }

            for (i = 0; i < 16; i++)
            {
                pwmWrite(PIN_BASE + i, 0);
                delay(20);
            }
        }

        pwmWrite(PIN_BASE + 16, 0);
        delay(500);
    }

    return 0;
}
