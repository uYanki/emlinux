// g++ -o app *.cpp ../../lib/*.cpp -I../../lib -lwiringPi -lpthread
// su ./app

//-----------------------------------------------------------------------------
//

// recommend to use vofa+ to display the curve
#define CONFIG_UART_PORT "/dev/ttyS1"

//-----------------------------------------------------------------------------
//

#include <iostream>
#include <unistd.h>
#include <signal.h>

#ifdef CONFIG_UART_PORT
#include "serial.h"
#endif

#include "max30102.h"

using namespace std;

MAX30102 max30102;

void fin(int sig)
{
    max30102.shutDown();
    std::cout << "Stop sampler" << std::endl;
    exit(0);
}

int main(void)
{
#ifdef CONFIG_UART_PORT

    int serial = serialOpen(CONFIG_UART_PORT, 115200);  // fd

    if (serial < 0)
    {
        std::cout << "Failed to open " << CONFIG_UART_PORT << std::endl;
        return (-1);
    }

#endif

    // Listen to Ctrl-C using the function you want to activate
    signal(SIGINT, fin);

    int result = max30102.begin();
    if (result < 0)
    {
        std::cout << "Failed to start I2C (Error: " << result << ")." << std::endl;
        return (-1 * result);
    }
    std::cout << "Device found (revision: " << result << ")!" << std::endl;

    max30102.setup();
    // max30102.softReset();
    max30102.setPulseAmplitudeRed(0x0A);

    while (1)
    {
        float    temp = max30102.readTemperatureF();
        uint32_t ir   = max30102.getIR();
        uint32_t red  = max30102.getRed();

#ifdef CONFIG_UART_PORT

        static char txbuf[32] = {0};
        sprintf(txbuf, "%d,%d\n", ir, red);
        serialWrite(serial, txbuf, strlen(txbuf));

#else

        std::cout << ir << ", " << red << ", " << temp << std::endl;

#endif

        usleep(1000);
    }

    return 0;
}
