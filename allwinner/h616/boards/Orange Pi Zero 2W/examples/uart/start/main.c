// gcc *.c -o  app

#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#include "serial.h"

#define tty "/dev/ttyS1"

void* uart_read_thread(void* arg)
{
    int    fd = *((int*)arg);
    size_t size;
    char   r_data[256] = {0};

    while (1)
    {
        size = serialRead(fd, r_data, 256);  // 阻塞方式去读

        if (size > 0)
        {
            printf("rx: %s\n", r_data);
        }
    }
}

int main(int argc, char* argv)
{
    int fd = 0; /* File descriptor for the port */
    int ret;

    pthread_t m_read_thread;

    fd = serialOpen(tty, 115200);

    if (fd > 0)
    {
        printf("open " tty " ok\n");
    }
    else
    {
        printf("open " tty " fail\n");
        return -1;
    }

    ret = pthread_create(&m_read_thread, NULL, uart_read_thread, &fd);  // rx

    if (ret)
    {
        perror("pthread_create error\n");
        return -1;
    }

    // pthread_join(m_read_thread,NULL);

    while (1)
    {
        static char txbuf[10] = {0};
        static int  times     = 0;

        sprintf(txbuf, "%d\r\n", times++);
        serialWrite(fd, txbuf, strlen(txbuf));  // tx
        sleep(1);
    }
}
