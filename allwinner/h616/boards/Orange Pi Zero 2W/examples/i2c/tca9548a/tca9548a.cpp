

#include "tca9548a.h"
#include <wiringPiI2C.h>
#include <wiringPi.h>

TCA9548A::TCA9548A()
{
    wiringPiSetup();
}

TCA9548A::~TCA9548A()
{
    noChannel();
}

int TCA9548A::init(int id)
{
    return this->fd = wiringPiI2CSetup(id);
}

void TCA9548A::setChannel(uint8_t channel)
{
    wiringPiI2CWrite(this->fd, 1 << channel);
}

void TCA9548A::noChannel()
{
    wiringPiI2CWrite(this->fd, 0);
}
