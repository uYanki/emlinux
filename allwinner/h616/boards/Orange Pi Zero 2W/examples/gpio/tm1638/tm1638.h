/**
 * @section EXAMPLE
 *
 *     #include <bcm2835.h>
 *     #include <tm1638.h>
 *
 *     ...
 *
 *     if (!bcm2835_init())
 *       { ... }
 *
 *     tm1638_t* t = tm1638_alloc(17, 21, 22);
 *     if (!t)
 *       { ... }
 *
 *     tm1638_set_7seg_text(t, "Hello!", 0xc0);
 *
 *     while(...)
 *       {
 *         uint8_t  x = tm1638_read_8buttons(t);
 *         tm1638_set_8leds(t, 0, x);
 *       }
 *
 *
 */

#ifndef _TM1638_H_
#define _TM1638_H_

typedef struct {
    uint8_t data;      /**< The pin which is connected to the TM1638's data line */
    uint8_t clock;     /**< The pin which is connected to the TM1638's clock line */
    uint8_t strobe;    /**< The pin which is connected to the TM1638's strobe line */
    uint8_t intensity; /**< The current LED brightness */
    bool    enable;    /**< true iff we're enabled */
} tm1638_t;

/**
 * @param data   Pin used for data.
 * @param clock  Pin used for clock.
 * @param strobe Pin used for strobe.
 *
 * @return       Pointer to tm1638 struct, or NULL if failure!
 */
void tm1638_init(tm1638_t* t, uint8_t data, uint8_t clock, uint8_t strobe);

/**
 *
 * Enable/disable the display
 *
 * @param t       Pointer to the tm1638 of interest.
 * @param enable  Enable (true) or disable (false).
 */
void tm1638_enable(tm1638_t* t, bool enable);

/**
 *
 * Set the display intensity.
 *
 * @param t           Pointer to the tm1638 of interest.
 * @param intensity   The desired intensity (0-7).
 */
void tm1638_set_intensity(tm1638_t* t, uint8_t intensity);

/**
 *
 * Set segments in a particular digit.
 *
 * @param t       Pointer to the tm1638 of interest.
 * @param digit   The digit of interest (0 is left-most).
 * @param n       The segments to set: (1 is top central, 128 is the point).
 */
void tm1638_set_7seg_raw(const tm1638_t* t, uint8_t digit, uint8_t n);

/**
 *
 * Display some text on the display.
 *
 * @param t       Pointer to the tm1638 of interest.
 * @param str     The text to display.
 * @param dots    The 8 bits correspond to the decimal points, LSB = leftmost.
 */
void tm1638_set_7seg_text(const tm1638_t* t, const char* str, uint8_t dots);

/**
 *
 * Set the status of one LED
 *
 * @param t       Pointer to the tm1638 of interest.
 * @param led     The LED in question.
 * @param cols    The colour to which it should be set.
 */
void tm1638_set_led(const tm1638_t* t, uint8_t led, uint8_t cols);

/**
 *
 * Set the status of all eight LEDs at once
 *
 * @param t       Pointer to the tm1638 of interest.
 * @param state     A byte's worth of red data (MSB is leftmost).
 *
 * The ordering might seem odd, but makes the display a sensible
 * binary rendition of red & green.
 */
void tm1638_set_8leds(const tm1638_t* t, uint8_t state);

/**
 *
 * Turn off all the LEDs
 *
 * @param t       Pointer to the tm1638 of interest.
 */
void tm1638_send_cls(const tm1638_t* t);

/**
 *
 * A simple 7-segment font.
 *
 * @param c       The ASCII character of interest.
 *
 * @return        The segments to set.
 */
uint8_t tm1638_font(char c);

/**
 *
 * Read the 32-bit button input register.
 * The bit order in here isn't helpful for the standard
 * boards with eight buttons on them: see tm1638_read_8buttons()
 * instead.
 *
 * @param t       Pointer to the tm1638 of interest.
 *
 * @return        32-bit uint of button states.
 */
uint32_t tm1638_read_buttons(const tm1638_t* t);

/**
 *
 * Read the state of the eight buttons on the standard board.
 *
 * @param t       Pointer to the tm1638 of interest.
 *
 * @return        8-bit uint of button states. MSB is leftmost.
 */
uint8_t tm1638_read_8buttons(const tm1638_t* t);

#endif
