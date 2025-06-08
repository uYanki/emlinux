#pragma once

#include <wiringPi.h>
#include <pcf8574.h>
#include <stdio.h>
#include <lcd.h>

#ifndef __NEOPIZEL_H__
#define __NEOPIZEL_H__

// PCF8574 Start I/O address
//  PCF8754 64+8
#define AF_BASE 64
#define AF_RS   (AF_BASE + 0)
#define AF_RW   (AF_BASE + 1)
#define AF_E    (AF_BASE + 2)
#define AF_LED  (AF_BASE + 3)
#define AF_DB4  (AF_BASE + 4)
#define AF_DB5  (AF_BASE + 5)
#define AF_DB6  (AF_BASE + 6)
#define AF_DB7  (AF_BASE + 7)

#ifdef __cplusplus
extern "C" {
#endif

int  lcdSetup(unsigned int i2cAddress);
void lcdLED(int state);

#ifdef __cplusplus
}
#endif
#endif
