// g++ -o app *.cpp ../../lib/*.cpp -I../../lib -lwiringPi -lpthread
// su ./app

//-----------------------------------------------------------------------------
//

#include <iostream>
#include <unistd.h>
#include "HeartRate.h"

using namespace std;

int main()
{
    MAX30102  sensor;
    HeartRate heartRate(&sensor);

    heartRate.begin();

    while (1)
    {
        float tempC = (sensor.readTemperatureF() - 32) / 1.8;  // °C

        std::cout << "\33[2K\r";  // clear line
        // std::cout << "\033c";     // clear all

        std::cout << heartRate.getLatestIRHeartRate() << ", "
                  << heartRate.getSafeIRHeartRate() << ", "
                  << tempC;

        std::cout << std::flush;

        usleep(100000);
    }
    return 0;
}
