#ifndef __SERIAL_H__
#define __SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

speed_t getBaudRate(int baudRate);
int     setParity(int fd, int dataBits, int stopBits, int parity);
int     serialOpen(const char* path, int baudRate);
int     serialWrite(int fd, char* writeData, int len);
int     serialRead(int fd, char* readData, int len);
int     serialClose(int fd);

#ifdef __cplusplus
}
#endif

#endif