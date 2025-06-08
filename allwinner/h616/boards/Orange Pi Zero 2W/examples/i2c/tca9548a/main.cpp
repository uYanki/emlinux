// g++ *.cpp -o app -lwiringPi

#include <stdlib.h>  // system
#include "tca9548a.h"

#define I2C_SACN "i2cdetect -y 2"

int main(void)
{
    TCA9548A tca;

    tca.init();

    for (uint8_t i = 0; i < 8; ++i)
    {
        tca.setChannel(i);

        // scan i2c address
        system(I2C_SACN);
    }
}