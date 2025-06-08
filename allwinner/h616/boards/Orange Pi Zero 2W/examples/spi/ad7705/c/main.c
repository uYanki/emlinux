#include "ad7705.h"

int main(int argc, char* argv[])
{
    int fd = spi_init("/dev/spidev1.1", SPI_CPHA | SPI_CPOL);

    int no_tty = !isatty(fileno(stdout));

    ad7705_init(fd);

    while (1)
    {
        while (ad7705_is_ready(fd) == false)
            ;

        int value = ad7705_read_adc(fd, 1);
        fprintf(stderr, "voltage = %.4f (V)\r", value / 65535.f * 5);

        if (no_tty)
        {
            printf("%d\n", value);
            fflush(stdout);
        }
    }

    spi_deinit(fd);

    return 0;
}
