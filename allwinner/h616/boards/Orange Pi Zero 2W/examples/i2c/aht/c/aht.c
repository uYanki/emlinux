

#include "aht.h"
#include <stdio.h>
#include <time.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

// commands
#define AHT_INIT_CMD    0xE1
#define AHT_SOFT_RESET  0xBA
#define AHT_TRIG_MEAS   0xAC
#define AHT_DAT1_CMD    0x33
#define AHT_DAT2_CMD    0x00

#define aht_log         printf

// Commands to trigger measurement
static const uint8_t sht_cmd_trig[3]  = {AHT_TRIG_MEAS, AHT_DAT1_CMD, AHT_DAT2_CMD};
// Calibration commands
static const uint8_t sht_cmd_calib[3] = {0xE1, 0x08, 0x00};

static int fd = -1;

bool aht_init(const char* device, uint8_t address)
{
    // Open I2C Bus
    if ((fd = open(device, O_RDWR)) < 0)
    {
        aht_log("Failed to open the i2c bus");
        return false;
    }

    if (ioctl(fd, I2C_SLAVE, address) < 0)
    {
        aht_log("Failed to acquire bus access and/or talk to slave.\n");
        return false;
    }

    // Calibrate
    if (write(fd, sht_cmd_calib, ARRAY_SIZE(sht_cmd_calib)) != ARRAY_SIZE(sht_cmd_calib))
    {
        aht_log("Failed to write to the i2c bus.\n");
        return false;
    }

    return true;
}

bool aht_read(float* temp, float* humi)
{
    uint8_t rxbuf[6];

    // Send trigger command
    if (write(fd, sht_cmd_trig, ARRAY_SIZE(sht_cmd_trig)) != ARRAY_SIZE(sht_cmd_trig))
    {
        aht_log("Failed to write to the i2c bus.\n");
        return false;
    }

    // The sensor needs 75 ms before reading the data
    aht_delay(75);

    // Read data
    if (read(fd, rxbuf, ARRAY_SIZE(rxbuf)) != ARRAY_SIZE(rxbuf))
    {
        aht_log("Failed to read from the i2c bus.\n");
        return false;
    }

    // check if the device is busy and try to read data again
    while (rxbuf[0] & (1 << 7))
    {
        if (read(fd, rxbuf, ARRAY_SIZE(rxbuf)) != ARRAY_SIZE(rxbuf))
        {
            aht_log("Failed to read from the i2c bus.\n");
            return false;
        }
    }

    uint32_t humi_raw = ((uint32_t)rxbuf[1] << 12) | ((uint16_t)rxbuf[2] << 4) | (rxbuf[3] >> 4);
    uint32_t temp_raw = (((uint32_t)rxbuf[3] & 0x0F) << 16) | ((uint16_t)rxbuf[4] << 8) | (rxbuf[5]);

    *humi = (((float)humi_raw) / (1 << 20)) * 100;
    *temp = ((((float)temp_raw) / (1 << 20)) * 200) - 50;

    return true;
}

void aht_delay(long ms)
{
    long    pause;
    clock_t now, then;
    pause = ms * (CLOCKS_PER_SEC / 1000);
    now = then = clock();
    while ((now - then) < pause)
    {
        now = clock();
    }
}
