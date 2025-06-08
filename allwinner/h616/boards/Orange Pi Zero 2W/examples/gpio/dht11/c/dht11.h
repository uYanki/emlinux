#ifndef __DHT11_H__
#define __DHT11_H__

#include <wiringPi.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

bool dht11_read_data(uint8_t* temp, uint8_t* humi);

#endif  // !__DHT11_H__