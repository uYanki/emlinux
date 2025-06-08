//  gcc -o app *.c -lwiringPi -lpthread

#include <stdlib.h>
#include <stdio.h>
#include "ad9833.h"

int main()
{
    wiringPiSetup();

    pullUpDnControl(16, PUD_UP);  // SPI_CS -> ad9833 fsync
    pullUpDnControl(11, PUD_UP);  // SPI_MOSI
    pullUpDnControl(14, PUD_UP);  // SPI_CLK

    while (1)
    {
        ad9833_init();
        ad9833_set_frequency(AD9833_FREQ0_SELECT, 1000);
        ad9833_set_phase(AD9833_PHASE0_SELECT, 0);
        ad9833_set_output(AD9833_FREQ0_OUTPUT, AD9833_PHASE0_OUTPUT);

        printf("sine\n");  // 1000Hz

        ad9833_set_waveform(AD9833_WAVEFORM_SINE);
        delay(5000);
        printf("half square\n");  // 504Hz

        ad9833_set_waveform(AD9833_WAVEFORM_HALF_SQUARE);
        delay(5000);

        printf("square\n");  // 1000Hz
        ad9833_set_waveform(AD9833_WAVEFORM_SQUARE);
        delay(5000);

        printf("triangle\n");  // 1000Hz
        ad9833_set_waveform(AD9833_WAVEFORM_TRIANGLE);
        delay(5000);
    }
}
