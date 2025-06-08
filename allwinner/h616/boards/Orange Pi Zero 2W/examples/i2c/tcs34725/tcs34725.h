

#ifndef __TCS34725_H__
#define __TCS34725_H__

#include "i2cbus.h"

#define ADDRESS           0x29

#define TCS34721_IDENTIFY 0x4D  // 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727
#define TCS34723_IDENTIFY 0x4D  // 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727
#define TCS34725_IDENTIFY 0x4D  // 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727
#define TCS3472_IDENTIFY  0x4D  // 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727

#define COMMAND_BIT       0x80

enum Register {
    ENABLE  = 0x80,
    ATIME   = 0x81,
    WTIME   = 0x83,
    AILTL   = 0x84,
    AILTH   = 0x85,
    AIHTL   = 0x86,
    AIHTH   = 0x87,
    PERS    = 0x8C,
    CONFIG  = 0x8D,
    CONTROL = 0x8F,
    ID      = 0x92,
    STATUS  = 0x93,
    CDATAL  = 0x94,
    CDATAH  = 0x95,
    RDATAL  = 0x96,
    RDATAH  = 0x97,
    GDATAL  = 0x98,
    GDATAH  = 0x99,
    BDATAL  = 0x9A,
    BDATAH  = 0x9B
};

enum Enable {
    AIEN = 0x10,
    WEN  = 0x08,
    AEN  = 0x02,
    PON  = 0x01
};

enum WTime {
    //_2_4MS = 0xFF,
    _204MS = 0xAB,
    _614MS = 0x00
};

enum Pers {
    NONE      = 0b0000,
    _1_CYCLE  = 0b0001,
    _2_CYCLE  = 0b0010,
    _3_CYCLE  = 0b0011,
    _5_CYCLE  = 0b0100,
    _10_CYCLE = 0b0101,
    _15_CYCLE = 0b0110,
    _20_CYCLE = 0b0111,
    _25_CYCLE = 0b1000,
    _30_CYCLE = 0b1001,
    _35_CYCLE = 0b1010,
    _40_CYCLE = 0b1011,
    _45_CYCLE = 0b1100,
    _50_CYCLE = 0b1101,
    _55_CYCLE = 0b1110,
    _60_CYCLE = 0b1111
};

enum Config {
    WLONG = 0x02
};

enum Status {
    AINT   = 0x10,
    AVALID = 0x01
};

enum IntegrationTime {
    _2_4MS = 0xFF, /**<  2.4ms - 1 cycle    - Max Count: 1024  */
    _24MS  = 0xF6, /**<  24ms  - 10 cycles  - Max Count: 10240 */
    _50MS  = 0xEB, /**<  50ms  - 20 cycles  - Max Count: 20480 */
    _101MS = 0xD5, /**<  101ms - 42 cycles  - Max Count: 43008 */
    _154MS = 0xC0, /**<  154ms - 64 cycles  - Max Count: 65535 */
    _700MS = 0x00  /**<  700ms - 256 cycles - Max Count: 65535 */
};

enum Gain {
    _1X  = 0x00,
    _4X  = 0x01,
    _16X = 0x02,
    _60X = 0x03
};

class TCS34725 : public I2C {
public:
    TCS34725(IntegrationTime = _2_4MS, Gain = _1X);

    bool isIdentify(void);

    void            setIntegrationTime(IntegrationTime);
    IntegrationTime getIntegrationTime(void);

    void setGain(Gain);
    Gain getGain(void);

    void getRawData(void);

    uint16_t calculateColorTemperature(void);
    uint16_t calculateLux(void);

    void setInterrupt(bool);
    void clearInterrupt(void);
    void setIntLimits(uint16_t, uint16_t);

    uint16_t getClear(void);
    uint16_t getRed(void);
    uint16_t getGreen(void);
    uint16_t getBlue(void);
    uint32_t getRGB888(void);

    virtual ~TCS34725();

private:
    bool            initialised = false;
    Gain            gain;
    IntegrationTime integrationTime;

    uint16_t clear;
    uint16_t red;
    uint16_t green;
    uint16_t blue;

    void enable(void);
    void disable(void);
};

#endif