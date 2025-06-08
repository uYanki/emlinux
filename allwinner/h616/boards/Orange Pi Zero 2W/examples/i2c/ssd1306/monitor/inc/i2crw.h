#include <wiringPi.h>
#include <wiringPiI2C.h> 
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <stdint.h>
#include <fcntl.h>

int I2C_ReadMem(int fd, int reg, uint8_t* buf, int size);
int I2C_WirteMem(int fd, int reg, uint8_t* buf, int size);