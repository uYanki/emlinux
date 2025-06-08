
#include <stdio.h>
#include "aht.h"

int main()
{
    aht_init("/dev/i2c-2", AHT_ADDR);

    float temp, humi;

    while (1)
    {
        if (aht_read(&temp, &humi))
        {
            printf("\rT: %.2f C, H: %.2f %%", temp, humi);
            fflush(stdout);
        }

        aht_delay(1000);
    }

    return 0;
}
