#include "dht11.h"

//-----------------------------------------------------------------------------
// config

#define PIN_DHT11         27

// PowerSupply: 3V3/5V

//-----------------------------------------------------------------------------
// port

#define DHT11_PULL_UP()   pullUpDnControl(PIN_DHT11, PUD_UP)

#define DHT11_DQ_IN()     pinMode(PIN_DHT11, INPUT)
#define DHT11_DQ_OUT()    pinMode(PIN_DHT11, OUTPUT)

#define DHT11_DQ_SET_L()  digitalWrite(PIN_DHT11, LOW)
#define DHT11_DQ_SET_H()  digitalWrite(PIN_DHT11, HIGH)

#define DHT11_DQ_GET_X()  digitalRead(PIN_DHT11)
#define DHT11_DQ_GET_L()  (digitalRead(PIN_DHT11) == LOW)
#define DHT11_DQ_GET_H()  (digitalRead(PIN_DHT11) == HIGH)

#define DHT11_SLEEP_MS(t) delay(t)
#define DHT11_SLEEP_US(t) delayMicroseconds(t)

//-----------------------------------------------------------------------------

void dht11_reset(void)
{
    DHT11_PULL_UP();
    DHT11_DQ_OUT();

    // master set low at least 18ms
    DHT11_DQ_SET_L();
    DHT11_SLEEP_MS(20);

    // master set high 20~40us
    DHT11_DQ_SET_H();
    DHT11_SLEEP_US(35);
}

/**
 * @brief Check if DHT11 exists
 */
bool dht11_check(void)
{
    const uint8_t timeout = 100;

    uint8_t times = 0;

    DHT11_DQ_IN();

    // wait slave set low 40~80us

    while (DHT11_DQ_GET_H())
    {
        if (times < timeout)
        {
            times++;
            DHT11_SLEEP_US(1);
        }
        else
        {
            return false;
        }
    }

    // wait slave set high 40~80us

    while (DHT11_DQ_GET_L())
    {
        if (times < timeout)
        {
            times++;
            DHT11_SLEEP_US(1);
        }
        else
        {
            return false;
        }
    }

    return true;
}

static uint8_t dht11_read_bit(void)
{
    uint8_t times = 0;

    const uint8_t timeout = 100;

    DHT11_DQ_IN();

    // wait low level
    while (DHT11_DQ_GET_H() && times < timeout)
    {
        times++;
        DHT11_SLEEP_US(1);
    }

    times = 0;

    // wait high level
    while (DHT11_DQ_GET_L() && times < timeout)
    {
        times++;
        DHT11_SLEEP_US(1);
    }

    DHT11_SLEEP_US(40);

    return DHT11_DQ_GET_X();
}

static uint8_t dht11_read_byte(void)
{
    uint8_t idx, dat;

    dat = 0;

    for (idx = 0; idx < 8; idx++)
    {
        dat <<= 1;
        dat |= dht11_read_bit();
    }

    return dat;
}

/**
 * @param temp 温度值, 0~50℃
 * @param humi 湿度值, 20%~90%
 */
bool dht11_read_data(uint8_t* temp, uint8_t* humi)
{
    uint8_t i;
    uint8_t buf[5];

    dht11_reset();

    if (dht11_check() == false)
    {
        return false;
    }

    for (i = 0; i < 5; i++)  // 读取40位数据
    {
        buf[i] = dht11_read_byte();
    }

    if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
    {
        *humi = buf[0];
        *temp = buf[2];

        return true;
    }
    else
    {
        return false;
    }
}
