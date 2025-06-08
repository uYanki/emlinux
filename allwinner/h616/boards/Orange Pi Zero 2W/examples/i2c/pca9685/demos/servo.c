/*************************************************************************
 * servo.c
 *
 * PCA9685 servo example
 * Connect a servo to any pin. It will rotate to random angles.
 *
 **************************************************************************
 */

#include "pca9685.h"

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>

#define I2CDEV   "/dev/i2c-2"
#define PIN_BASE 300  // 虚拟引脚
#define MAX_PWM  4096
#define HERTZ    50

/**
 * Calculate the number of ticks the signal should be high for the required amount of time
 */
int calcTicks(float impulseMs, int hertz)
{
    float cycleMs = 1000.0f / hertz;  // 占空比=脉冲高电平时间/脉冲周期
    return (int)(MAX_PWM * impulseMs / cycleMs + 0.5f);
}

int main(void)
{
    printf("PCA9685 servo example\n");
    printf("Connect a servo to any pin. It will rotate to random angles\n");

    // Calling wiringPi setup first.
    wiringPiSetup();

    // Setup with pinbase 300 and i2c location 0x40
    int fd = pca9685Setup(I2CDEV, PIN_BASE, 0x40, HERTZ);
    printf(I2CDEV "\n");
    if (fd < 0)
    {
        printf("Error in setup\n");
        return fd;
    }

    // Reset all output
    pca9685PWMReset(fd);

    short angle = 0, dir = 0, step = 10;

    while (1)
    {
        if (dir == 0)
        {
            // inc
            angle += step;

            if (angle > 180)
            {
                angle = 180;
                dir   = !dir;
            }
        }
        else
        {
            // dec

            angle -= step;

            if (angle < 0)
            {
                angle = 0;
                dir   = !dir;
            }
        }

        float millis = angle / 90.f;  // 0~180 => 0~2
        int   tick   = calcTicks(millis, HERTZ);

        pwmWrite(PIN_BASE + 16, tick);
        delay(100);
    }

    return 0;
}
