#ifndef __I2C_BUS_H__
#define __I2C_BUS_H__

#include <stdint.h>

class I2C {
public:
    I2C();
    I2C(uint16_t);
    void init(uint16_t);

    bool isInitialize(void);

    int8_t read_byte(void);
    bool   write_byte(uint8_t);

    int8_t read_byte(uint8_t);
    bool   write_byte(uint8_t, uint8_t);

    int16_t read_word(uint8_t);
    bool    write_word(uint8_t, uint16_t);

    int16_t read_word_swapped(uint8_t);
    bool    write_word_swapped(uint8_t, uint16_t);

    virtual ~I2C();

private:
    int  handle;
    bool initialize = false;

    union i2cUData {
        uint16_t uSData;
        uint8_t  uCData[2];
    };

    union i2cData {
        int16_t uSData;
        int8_t  uCData[2];
    };

    void swap(i2cUData&);
    void swap(i2cData&);
};

#endif