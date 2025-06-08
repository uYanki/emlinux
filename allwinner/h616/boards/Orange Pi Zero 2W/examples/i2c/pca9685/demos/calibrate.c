/*************************************************************************
 * calibrate.c
 *
 * PCA9685 servo calibration
 * Use this to test out the min and max millisecond values of your servo
 *
 **************************************************************************
 */

#include "pca9685.h"

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#define I2CDEV   "/dev/i2c-2"
#define PIN_BASE 300
#define MAX_PWM  4096
#define HERTZ    50

/**
 * Calculate the number of ticks the signal should be high for the required amount of time
 */
int calcTicks(float impulseMs, int hertz)
{
    float cycleMs = 1000.0f / hertz;
    return (int)(MAX_PWM * impulseMs / cycleMs + 0.5f);
}

int main(void)
{
    printf("PCA9685 servo calibration\n");
    printf("Use this to test out the min and max millisecond values of your servo\n");

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
    printf("Frequency is set to %d hertz\n", HERTZ);

    int   i, j = 1;
    int   pin;
    float millis;

    while (j)
    {
        printf("Enter servo pin [0-16]: ");
        scanf("%d", &pin);

        if (pin >= 0 && pin <= 16)
        {
            millis = 1.5f;
            i      = 1;

            pwmWrite(PIN_BASE + pin, calcTicks(millis, HERTZ));
            printf("Servo %d is centered at %1.2f ms\n", pin, millis);

            while (i)
            {
                printf("Enter milliseconds: ");
                scanf("%f", &millis);

                if (millis > 0 && millis <= 3)
                {
                    pwmWrite(PIN_BASE + pin, calcTicks(millis, HERTZ));
                    delay(1000);
                }
                else
                {
                    i = 0;
                }
            }
        }
        else
        {
            j = 0;
        }
    }

    return 0;
}
