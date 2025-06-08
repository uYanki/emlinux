
#ifndef __HCSR04_H__
#define __HCSR04_H__

#include "gpio.h"
#include <chrono>  // Chronometre

class HCSR04 {
public:
    HCSR04(Pin, Pin);
    virtual ~HCSR04();

    void   setTemperature(double);
    double getDistance(void);

    void startMeasure(void);
    void procMeasure(void);
    bool isMeasureOk(void);
    GPIO getEcho(void);

private:
    GPIO trigger;
    GPIO echo;

    bool   mesaureOk   = false;
    double temperature = 20.0;
    double distance;
};

#endif