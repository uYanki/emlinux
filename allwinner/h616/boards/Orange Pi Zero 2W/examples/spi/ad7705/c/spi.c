
#include "spi.h"

// const char* spi_device = "/dev/spidev1.1";
// uint8_t     spi_mode   = SPI_CPHA | SPI_CPOL;

uint8_t  spi_bits  = 8;
uint32_t spi_speed = 50000;
uint16_t spi_delay = 10;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

//-----------------------------------------------------------------------------
//

void pabort(const char* s)
{
    perror(s);
    abort();
}

int spi_init(const char* device, uint8_t mode)
{
    int ret;
    int fd;

    fd = open(device, O_RDWR);
    if (fd < 0)
    {
        pabort("can't open device");
    }

    /*
     * spi mode
     */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
    {
        pabort("can't set spi mode");
    }

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
    {
        pabort("can't get spi mode");
    }

    /*
     * bits per word
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bits);
    if (ret == -1)
    {
        pabort("can't set bits per word");
    }

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bits);
    if (ret == -1)
    {
        pabort("can't get bits per word");
    }

    /*
     * max speed hz
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if (ret == -1)
    {
        pabort("can't set max speed hz");
    }

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
    if (ret == -1)
    {
        pabort("can't get max speed hz");
    }

    fprintf(stderr, "spi mode: %d\n", mode);
    fprintf(stderr, "bits per word: %d\n", spi_bits);
    fprintf(stderr, "max speed: %d Hz (%d KHz)\n", spi_speed, spi_speed / 1000);

    return fd;
}

void spi_write_byte(int fd, uint8_t val)
{
    int     ret;
    uint8_t tx[1] = {val};
    uint8_t rx[1] = {0};

    struct spi_ioc_transfer tr = {
        .tx_buf        = (unsigned long)tx,
        .rx_buf        = (unsigned long)rx,
        .len           = ARRAY_SIZE(tx),
        .delay_usecs   = spi_delay,
        .speed_hz      = spi_speed,
        .bits_per_word = spi_bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        pabort("can't send spi message");
    }
}

uint8_t spi_read_byte(int fd)
{
    int     ret;
    uint8_t tx[1] = {0};
    uint8_t rx[1] = {0};

    struct spi_ioc_transfer tr = {
        .tx_buf        = (unsigned long)tx,
        .rx_buf        = (unsigned long)rx,
        .len           = ARRAY_SIZE(tx),
        .delay_usecs   = spi_delay,
        .speed_hz      = spi_speed,
        .bits_per_word = 8,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        pabort("can't send spi message");
    }

    return rx[0];
}

int spi_read_word(int fd)
{
    int     ret;
    uint8_t tx[2] = {0, 0};
    uint8_t rx[2] = {0, 0};

    struct spi_ioc_transfer tr = {
        .tx_buf        = (unsigned long)tx,
        .rx_buf        = (unsigned long)rx,
        .len           = ARRAY_SIZE(tx),
        .delay_usecs   = spi_delay,
        .speed_hz      = spi_speed,
        .bits_per_word = 8,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        pabort("can't send spi message");
    }

    return (rx[0] << 8) | (rx[1]);
}

void spi_deinit(int fd)
{
    close(fd);
}