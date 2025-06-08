
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <wiringPi.h>
#include "tm1638.h"

static void    tm1638_send_raw(const tm1638_t* t, uint8_t x);
static uint8_t tm1638_receive_raw(const tm1638_t* t);
static void    tm1638_send_command(const tm1638_t* t, uint8_t x);
static void    tm1638_send_data(const tm1638_t* t, uint8_t addr, uint8_t data);
static void    tm1638_send_config(const tm1638_t* t);
static uint8_t tm1638_calc_config(const tm1638_t* t);

void tm1638_init(tm1638_t* t, uint8_t data, uint8_t clock, uint8_t strobe)
{
    /* The delays in this code are somewhat arbitrary: they work for me
       but I make no claims that they are optimal or robust */

    t->data   = data;
    t->clock  = clock;
    t->strobe = strobe;

    t->intensity = 7;
    t->enable    = true;

    pinMode(t->data, OUTPUT);
    pinMode(t->clock, OUTPUT);
    pinMode(t->strobe, OUTPUT);

    digitalWrite(t->strobe, HIGH);
    digitalWrite(t->clock, HIGH);
    delayMicroseconds(1);

    tm1638_send_config(t);

    tm1638_send_cls(t);
}

//
// Enable/disable the display
//

void tm1638_enable(tm1638_t* t, bool enable)
{
    t->enable = enable;
    tm1638_send_config(t);
}

//
// Set the display intensity.
//

void tm1638_set_intensity(tm1638_t* t, uint8_t intensity)
{
    /* maximum intensity is 7 */
    if (intensity > 7)
    {
        intensity = 7;
    }

    t->intensity = intensity;

    tm1638_send_config(t);
}

//
// Send the settings in t to the actual hardware.
//

static void tm1638_send_config(const tm1638_t* t)
{
    tm1638_send_command(t, tm1638_calc_config(t));
}

//
// Given settings in t, calculate the config byte to send.
//

static uint8_t tm1638_calc_config(const tm1638_t* t)
{
    return 0x80 | (t->enable ? 8 : 0) | t->intensity;
}

//
// Low-level command primitive: send a byte to the hardware.
//

static void tm1638_send_raw(const tm1638_t* t, uint8_t x)
{
    /* The delays in this code are somewhat arbitrary: they work for me
       but I make no claims that they are optimal or robust */
    for (int i = 0; i < 8; i++)
    {
        digitalWrite(t->clock, LOW);
        delayMicroseconds(1);

        digitalWrite(t->data, x & 1 ? HIGH : LOW);
        delayMicroseconds(1);

        x >>= 1;
        digitalWrite(t->clock, HIGH);
        delayMicroseconds(1);
    }
}

//
// Low-level command primitive: read a byte from the hardware.
//

static uint8_t tm1638_receive_raw(const tm1638_t* t)
{
    /* The delays in this code are somewhat arbitrary: they work for me
       but I make no claims that they are optimal or robust */

    uint8_t x = 0;

    /* Turn GPIO pin into an input */
    pinMode(t->data, INPUT);

    for (int i = 0; i < 8; i++)
    {
        x <<= 1;

        digitalWrite(t->clock, LOW);
        delayMicroseconds(1);

        uint8_t y = digitalRead(t->data);
        if (y & 1)
        {
            x |= 1;
        }
        delayMicroseconds(1);

        digitalWrite(t->clock, HIGH);
        delayMicroseconds(1);
    }

    /* Turn GPIO pin back into an output */
    pinMode(t->data, OUTPUT);

    return x;
}

//
// Medium-level command primitive: send a command to the hardware.
//

static void tm1638_send_command(const tm1638_t* t, uint8_t x)
{
    /* The delays in this code are somewhat arbitrary: they work for me
       but I make no claims that they are optimal or robust */
    digitalWrite(t->strobe, LOW);
    delayMicroseconds(1);

    tm1638_send_raw(t, x);

    digitalWrite(t->strobe, HIGH);
    delayMicroseconds(1);
}

//
// Medium-level command primitive: write a data byte to the hardware.
//

static void tm1638_send_data(const tm1638_t* t, uint8_t addr, uint8_t data)
{
    /* The delays in this code are somewhat arbitrary: they work for me
       but I make no claims that they are optimal or robust */
    tm1638_send_command(t, 0x44);

    digitalWrite(t->strobe, LOW);
    delayMicroseconds(1);

    tm1638_send_raw(t, 0xc0 | addr);
    tm1638_send_raw(t, data);

    digitalWrite(t->strobe, HIGH);
    delayMicroseconds(1);
}

//
// Set segments in a particular digit
//

void tm1638_set_7seg_raw(const tm1638_t* t, uint8_t digit, uint8_t n)
{
    tm1638_send_data(t, digit << 1, n);
}

//
// Set the status of one LED
//

void tm1638_set_7seg_text(const tm1638_t* t, const char* str, uint8_t dots)
{
    const char* p = str;

    for (int i = 0, j = 1; i < 8; i++, j <<= 1)
    {
        // We want the loop to finish, but don't walk over the end of the string
        char c = *p;

        if (c)
        {
            p++;
        }

        uint8_t f = tm1638_font(c);

        if (dots & j)
        {
            f |= 0x80;
        }

        tm1638_set_7seg_raw(t, i, f);
    }
}

void tm1638_set_led(const tm1638_t* t, uint8_t led, uint8_t cols)
{
    tm1638_send_data(t, (led << 1) + 1, cols);
}

void tm1638_set_8leds(const tm1638_t* t, uint8_t state)
{
    for (int i = 0, j = 0x80; i < 8; i++, j >>= 1)
    {
        tm1638_set_led(t, i, (state & j) ? 1 : 0);
    }
}

void tm1638_send_cls(const tm1638_t* t)
{
    /* The delays in this code are somewhat arbitrary: they work for me
       but I make no claims that they are optimal or robust */
    tm1638_send_command(t, 0x40);

    digitalWrite(t->strobe, LOW);
    delayMicroseconds(1);

    tm1638_send_raw(t, 0xc0);

    for (int i = 0; i < 16; i++)
    {
        tm1638_send_raw(t, 0x00);
    }

    digitalWrite(t->strobe, HIGH);
    delayMicroseconds(1);
}

uint8_t tm1638_font(char c)
{
    static const uint8_t f[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x86, 0x22, 0x7e, 0x6d, 0x00, 0x00, 0x02,
        0x30, 0x06, 0x63, 0x00, 0x04, 0x40, 0x80, 0x52,
        0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27,
        0x7f, 0x6f, 0x00, 0x00, 0x00, 0x48, 0x00, 0x53,
        0x5f, 0x77, 0x7f, 0x39, 0x3f, 0x79, 0x71, 0x3d,
        0x76, 0x06, 0x1f, 0x69, 0x38, 0x15, 0x37, 0x3f,
        0x73, 0x67, 0x31, 0x6d, 0x78, 0x3e, 0x2a, 0x1d,
        0x76, 0x6e, 0x5b, 0x39, 0x64, 0x0f, 0x00, 0x08,
        0x20, 0x5f, 0x7c, 0x58, 0x5e, 0x7b, 0x31, 0x6f,
        0x74, 0x04, 0x0e, 0x75, 0x30, 0x55, 0x54, 0x5c,
        0x73, 0x67, 0x50, 0x6d, 0x78, 0x1c, 0x2a, 0x1d,
        0x76, 0x6e, 0x47, 0x46, 0x06, 0x70, 0x01, 0x00};

    return (c > 127) ? 0 : f[(uint8_t)c];
}

uint32_t tm1638_read_buttons(const tm1638_t* t)
{
    /* The delays in this code are somewhat arbitrary: they work for me
       but I make no claims that they are optimal or robust */
    digitalWrite(t->strobe, LOW);
    delayMicroseconds(1);

    tm1638_send_raw(t, 0x42);

    uint32_t x = 0;
    for (int i = 0; i < 4; i++)
    {
        x <<= 8;
        x |= tm1638_receive_raw(t);
    }

    digitalWrite(t->strobe, HIGH);
    delayMicroseconds(1);

    return x;
}

uint8_t tm1638_read_8buttons(const tm1638_t* t)
{
    uint32_t x = tm1638_read_buttons(t);
    uint8_t  y = 0;

    for (int i = 0; i < 4; i++)
    {
        y <<= 1;

        if (x & 0x80000000)
        {
            y |= 0x10;
        }

        if (x & 0x08000000)
        {
            y |= 0x01;
        }

        x <<= 8;
    }

    return y;
}
