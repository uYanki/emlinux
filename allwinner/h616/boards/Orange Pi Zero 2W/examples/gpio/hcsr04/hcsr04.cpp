

#include <wiringPi.h>
#include <chrono>  // Chronometre
#include "hcsr04.h"

using namespace std;
using namespace std::chrono;

HCSR04::HCSR04(Pin pinTrigger, Pin pinEcho)
{
    trigger.init(pinTrigger);
    echo.init(pinEcho);
    echo.in();
    trigger.out();
    trigger.off();
}

GPIO HCSR04::getEcho(void)
{
    return echo;
}

void HCSR04::startMeasure(void)
{
    mesaureOk = false;  // flag

    trigger.off();
    delayMicroseconds(50);

    trigger.on();
    delayMicroseconds(10);
    trigger.off();
}

void HCSR04::setTemperature(double pTemperature)
{
    temperature = pTemperature;
}

bool HCSR04::isMeasureOk(void)
{
    return mesaureOk;
}

double HCSR04::getDistance(void)
{
    return distance;
}

void HCSR04::procMeasure(void)
{
    // wait low level

    high_resolution_clock::time_point start = high_resolution_clock::now();

    while (echo.isOn())
        ;

    high_resolution_clock::time_point end = high_resolution_clock::now();

    // calchigh level time

    duration<double> delta = duration_cast<chrono::duration<double>>(end - start);

    // calc distance

    double speed = 0.62 * temperature + 331.6;
    distance     = speed * delta.count() / 2.0;

    mesaureOk = true;  // flag
}

HCSR04::~HCSR04()
{
}