#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

void pabort(const char* s);

int     spi_init(const char* device, uint8_t mode);
void    spi_deinit(int fd);
void    spi_write_byte(int fd, uint8_t val);
uint8_t spi_read_byte(int fd);
int     spi_read_word(int fd);

#endif