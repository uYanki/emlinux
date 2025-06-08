#ifndef __TCA9548A_H__
#define __TCA9548A_H__

#include <stdint.h>

class TCA9548A {
public:
    TCA9548A();
    ~TCA9548A();
    int  init(int id = 0x70);
    void setChannel(uint8_t channel);
    void noChannel();

private:
    int fd;
};

#endif
