#include "serial.h"

speed_t getBaudRate(int baudRate)
{
    switch (baudRate)
    {
        case 0:
            return B0;
        case 50:
            return B50;
        case 75:
            return B75;
        case 110:
            return B110;
        case 134:
            return B134;
        case 150:
            return B150;
        case 200:
            return B200;
        case 300:
            return B300;
        case 600:
            return B600;
        case 1200:
            return B1200;
        case 1800:
            return B1800;
        case 2400:
            return B2400;
        case 4800:
            return B4800;
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        case 460800:
            return B460800;
        case 500000:
            return B500000;
        case 576000:
            return B576000;
        case 921600:
            return B921600;
        case 1000000:
            return B1000000;
        case 1152000:
            return B1152000;
        case 1500000:
            return B1500000;
        case 2000000:
            return B2000000;
        case 2500000:
            return B2500000;
        case 3000000:
            return B3000000;
        case 3500000:
            return B3500000;
        case 4000000:
            return B4000000;
        default:
            return -1;
    }
}

int setParity(int fd, int dataBits, int stopBits, int parity)
{
    struct termios options;
    if (tcgetattr(fd, &options) != 0)
    {
        printf("SetupSerial 1");
        return (-1);
    }
    options.c_cflag &= ~CSIZE;
    switch (dataBits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "Unsupported data size\n");
            return (-1);
    }

    switch (parity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB; /* Clear parity enable */
            options.c_iflag &= ~INPCK;  /* Enable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK; /* Disable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB; /* Enable parity */
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK; /* Disable parity checking */
            break;
        case 'S':
        case 's': /*as no parity */
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported parity\n");
            return (-1);
    }
    switch (stopBits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported stop bits\n");
            return (-1);
    }

    /* Set input parity option */
    if (parity != 'n')
    {
        options.c_iflag |= INPCK;
    }
    tcflush(fd, TCIFLUSH);
    options.c_cc[VTIME] = 0x01;
    options.c_cc[VMIN]  = 0xFF; /* Update the options and do it NOW */
    // qd to set raw mode, which is copied from web
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    options.c_cflag &= ~(CSIZE | PARENB);
    options.c_cflag |= CS8;

    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("SetupSerial 3");
        return (-1);
    }

    return 0;
}

int serialOpen(const char* path, int baudRate)
{
    int     fd;
    speed_t speed;

    /* Check arguments */
    {
        speed = getBaudRate(baudRate);
        if (speed == -1)
        {
            printf("get Baud rate error\n");
            return -1;
        }
    }
    {
        fd = open(path, O_RDWR);
        if (fd == -1)
        {
            printf("open serial error =%d\n", fd);
            return -1;
        }
    }
    /* Configure device */
    {
        struct termios cfg;
        if (tcgetattr(fd, &cfg))
        {
            printf("tcgetattr() failed\n");
            close(fd);
            return -1;
        }

        cfmakeraw(&cfg);
        cfsetispeed(&cfg, speed);
        cfsetospeed(&cfg, speed);

        if (tcsetattr(fd, TCSANOW, &cfg))
        {
            printf("tcsetattr() failed\n");
            close(fd);
            return -1;
        }
    }
    setParity(fd, 8, 1, 'N');
    printf("open Success==%d\n", fd);
    return fd;
}

int serialWrite(int fd, char* writeData, int len)
{
    if (fd > 0)
    {
        write(fd, writeData, len);
    }
    else
    {
        printf("[File]=%s[Function]=%s error\n", __FILE__, __FUNCTION__);
        return -1;
    }

    return 0;
}

int serialRead(int fd, char* readData, int len)
{
    size_t size = 0;
    if (fd > 0)
    {
        size = read(fd, readData, len);
    }
    else
    {
        printf("[File]=%s[Function]=%s error\n", __FILE__, __FUNCTION__);
        return -1;
    }
    return size;
}

int serialClose(int fd)
{
    close(fd);
    return 0;
}
