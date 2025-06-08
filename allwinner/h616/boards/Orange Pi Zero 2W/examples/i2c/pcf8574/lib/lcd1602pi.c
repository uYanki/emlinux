#include "lcd101rpi.h"

int lcdSetup(unsigned int i2cAddress)
{
    int i, ret;

    for (i = 0; i < 8; i++)
    {
        pinMode(AF_BASE + i, OUTPUT);  // Will expand the IO port as the output mode
    }

    ret = pcf8574Setup(AF_BASE, i2cAddress);
    if (ret != 1)
    {
        fprintf(stderr, "pcf8574Setup setup failed, ret=%d\n", ret);
        return -1;
    }

    int lcdHandle = lcdInit(2, 16, 4, AF_RS, AF_E, AF_DB4, AF_DB5, AF_DB6, AF_DB7, 0, 0, 0, 0);

    if (lcdHandle >= 0)
    {
        // sanatize display
        digitalWrite(AF_RW, 0);   // Set the R/Wall to a low level, LCD for the write state
        digitalWrite(AF_LED, 1);  // Open back light

        lcdDisplay(lcdHandle, 0);
        lcdClear(lcdHandle);
        lcdHome(lcdHandle);
        lcdCursorBlink(lcdHandle, 0);
        lcdCursor(lcdHandle, 0);
        lcdDisplay(lcdHandle, 1);
    }

    return lcdHandle;
}

void lcdLED(int state)
{
    digitalWrite(AF_LED, state);
    delay(10);
    digitalWrite(AF_LED, state);
}
