// g++ -o app *.cpp -lwiringPi -lpthread

#include <iostream>
#include <chrono>
#include <wiringPi.h>

#include "ina219.h"

using namespace std;

INA219 ina219;

int main()
{
#if 1
    cout << "init :" << ina219.init(ADDR_40) << endl;
    cout << "reset:" << ina219.reset() << endl;
    cout << "cal  :" << ina219.setCalibration_0_4A(_16V, B_12Bits_128S_69MS, S_12Bits_128S_69MS, ShuntAndBusVoltageContinuous) << endl;
    for (int compteur = 0;; compteur++)
    {
        bool conversion = false;

        while (!conversion)
        {
            conversion = ina219.isConversionOk();
        }

        // Mesure
        bool  overflow     = ina219.isOverflow();
        float shuntVoltage = ina219.getShuntVoltage_mV();
        float busVoltage   = ina219.getBusVoltage_V();
        float current      = ina219.getCurrent_mA();
        float power        = ina219.getPower_W();
        cout << "num              :" << compteur << endl;
        cout << "Conversion ok    :" << conversion << endl;
        cout << "Bus voltage V    :" << busVoltage << endl;
        cout << "Shunt voltage mv :" << shuntVoltage << endl;
        cout << "Shunt current mA :" << shuntVoltage / 0.1 << endl;
        cout << "Current mA       :" << current << endl;
        cout << "Overflow         :" << overflow << endl;
        cout << "Shunt power W    :" << busVoltage * (current / 1000.0) << endl;
        cout << "Power W          :" << power << endl;
        cout << endl;

        delay(1000);
    }
#else
    ina219.init(ADDR_40);
    ina219.reset();
    ina219.setCalibration_3_2A(_32V, B_12Bits_1S_532US, S_9Bits_1S_84US, BusVoltageContinuous);
    cout << "time, Bus voltage V" << endl;
    uint32_t initTime = static_cast<long int>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    for (int compteur = 0; compteur != 10000; compteur++)
    {
        uint32_t time = static_cast<long int>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        cout << time - initTime << "," << ina219.getBusVoltage_V() * 1000 << endl;
    }
#endif
}
