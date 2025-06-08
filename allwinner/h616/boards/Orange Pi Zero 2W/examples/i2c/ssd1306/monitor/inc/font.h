
#ifndef __FONT_H__
#define __FONT_H__

#include <stdint.h>

#define USE_FONT_6x8
#define USE_FONT_7x10
#define USE_FONT_11x18
#define USE_FONT_16x26

typedef struct {
    uint8_t         width;
    uint8_t         height;
    const uint16_t* bitmap;
} font_t;

#ifdef USE_FONT_6x8
extern const font_t Font6x8;
#endif

#ifdef USE_FONT_7x10
extern const font_t Font7x10;
#endif

#ifdef USE_FONT_11x18
extern const font_t Font11x18;
#endif

#ifdef USE_FONT_16x26
extern const font_t Font16x26;
#endif

#endif
