#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(void) {
    int temp_fd, humidity_fd;
    char temp_buf[32], humidity_buf[32];
    int temp, humidity;

    temp_fd = open("/sys/bus/iio/devices/iio:device0/in_temp_input", O_RDONLY);
    humidity_fd = open("/sys/bus/iio/devices/iio:device0/in_humidityrelative_input", O_RDONLY);

    if (temp_fd == -1 || humidity_fd == -1) {
        perror("Failed to open sensor data files");
        exit(-1);
    }

    while (1) {
        // Read temperature data
        lseek(temp_fd, 0, SEEK_SET);
        ssize_t temp_size = read(temp_fd, temp_buf, sizeof(temp_buf) - 1);
        if (temp_size == -1) {
            perror("Failed to read temperature data");
            exit(-1);
        }
        temp_buf[temp_size] = '\0';
        temp = atoi(temp_buf);

        // Read humidity data
        lseek(humidity_fd, 0, SEEK_SET);
        ssize_t humidity_size = read(humidity_fd, humidity_buf, sizeof(humidity_buf) - 1);
        if (humidity_size == -1) {
            perror("Failed to read humidity data");
            exit(-1);
        }
        humidity_buf[humidity_size] = '\0';
        humidity = atoi(humidity_buf);

        // Convert to float
        float temp_float = (float)temp / 1000;
        float humidity_float = (float)humidity / 1000;

        printf("Temperature: %.1f C, Humidity: %.1f %%RH\n", temp_float, humidity_float);

        sleep(1);
    }

    close(temp_fd);
    close(humidity_fd);

    return 0;
}
