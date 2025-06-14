#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
 
int main()
{
    struct input_absinfo absI;
    int ret;
    int fd=-1;
    fd = open("/dev/input/event0",O_RDONLY);
    if(fd<0){
       perror("/dev/input/event0"); 
       return -1;
    }
   //得到X轴的abs信息
    ioctl(fd,EVIOCGABS(ABS_X),&absI);
    printf("x abs lastest value=%d\n",absI.value);
    printf("x abs min=%d\n",absI.minimum);
    printf("x abs max=%d\n",absI.maximum);
   //得到y轴的abs信息
    ioctl(fd,EVIOCGABS(ABS_Y),&absI);
    printf("y abs lastest value=%d\n",absI.value);
    printf("y abs min=%d\n",absI.minimum);
    printf("y abs max=%d\n",absI.maximum);
   //得到按压轴的abs信息
    ioctl(fd,EVIOCGABS(ABS_PRESSURE),&absI);
    printf("pressure abs lastest value=%d\n",absI.value);
    printf("pressure abs min=%d\n",absI.minimum);
    printf("pressure abs max=%d\n",absI.maximum);
    close(fd);
    return 0;
}
