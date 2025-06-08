#ifndef __AD7705_H__
#define __AD7705_H__

#include "spi.h"

void ad7705_init(int fd);
int  ad7705_read_adc(int fd, uint8_t ch);
void ad7705_calib_self(int fd, uint8_t ch);
void ad7705_sytem_calib_zero(int fd, uint8_t ch);
void ad7705_sytem_calib_full(int fd, uint8_t ch);
bool ad7705_is_ready(int fd);

#endif