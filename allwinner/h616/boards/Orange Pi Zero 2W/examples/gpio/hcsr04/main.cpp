
// g++ *.cpp -o app -lwiringPi

#include <iostream>
#include <iomanip>
#include "hcsr04.h"

int main()
{
    // 3v3 power supply
    HCSR04 hcsr04(GPIO23 /* trig */, GPIO22 /* echo */);

    GPIO echo = hcsr04.getEcho();

    while (1)
    {
        hcsr04.startMeasure();

        while (echo.isOff())
            ;

        // or call procMeasure() in echo rising irq
        hcsr04.procMeasure();

        std::cout << "\33[2K\r";  // clear line
        std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2) << hcsr04.getDistance() << " m";
        std::cout << std::flush;

        delay(100);
    }

    return 0;
}
