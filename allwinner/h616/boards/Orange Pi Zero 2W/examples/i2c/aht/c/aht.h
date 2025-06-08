
#ifndef __AHT_H__
#define __AHT_H__

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// Address
#define AHT_ADDR 0x38

// APIs
bool aht_init(const char* device, uint8_t address);
bool aht_read(float* temperature, float* humidity);
void aht_delay(long ms);

#endif /* __AHT_H__ */
