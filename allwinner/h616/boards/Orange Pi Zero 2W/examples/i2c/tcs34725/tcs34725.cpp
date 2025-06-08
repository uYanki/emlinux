

#include <stdlib.h>
#include <math.h>
#include <wiringPi.h>

#include "tcs34725.h"

TCS34725::TCS34725(IntegrationTime it, Gain gain)
{
    init(ADDRESS);

    integrationTime = it;
    gain            = gain;

    if (isIdentify())
    {
        initialised = true;

        /* Set default integration time and gain */
        setIntegrationTime(integrationTime);
        setGain(gain);

        /* Note: by default, the device is in power down mode on bootup */
        enable();
    }
}

// Enables device
void TCS34725::enable(void)
{
    write_byte(ENABLE, PON);
    delay(3);
    write_byte(ENABLE, PON | AEN);
}

// Disables device (putting it in lower power sleep mode)
void TCS34725::disable(void)
{
    /* Turn the device off to save power */
    uint8_t reg = 0;
    reg         = read_byte(ENABLE);
    write_byte(ENABLE, reg & ~(PON | AEN));
}

// Verifie l'identite du circuit
// @return true si c'est bien un TCS34725, sinon false
bool TCS34725::isIdentify(void)
{
    switch (read_byte(ID))
    {
        // 0x44 = TCS34721/TCS34725
        // 0x4D = TCS34723/TCS34727
        case 0x44:
        case 0x4D:
            return true;
        default:
            return false;
    }
}

// Sets the integration time
// @Param le temps d'integration
void TCS34725::setIntegrationTime(IntegrationTime it)
{
    /* Update the timing register */
    write_byte(ATIME, it);

    /* Update value placeholders */
    integrationTime = it;
}

// Get the integration time
// @Return le temps d'integration
IntegrationTime TCS34725::getIntegrationTime(void)
{
    return integrationTime;
}

// Adjusts the gain (adjusts the sensitivity to light)
// @Param le gain
void TCS34725::setGain(Gain pGain)
{
    /* Update the timing register */
    write_byte(CONTROL, pGain);

    /* Update value placeholders */
    gain = pGain;
}

// Get the gain
// @Return le gain
Gain TCS34725::getGain(void)
{
    return gain;
}

// Reads the raw red, green, blue and clear channel values
void TCS34725::getRawData(void)
{
    clear = read_word(CDATAL);
    red   = read_word(RDATAL);
    green = read_word(GDATAL);
    blue  = read_word(BDATAL);

    // Temporise la prochaine mesure pour
    // assurer que le temps d'integration est ecoule
    switch (integrationTime)
    {
        case _2_4MS:
            delay(3);
            break;
        case _24MS:
            delay(24);
            break;
        case _50MS:
            delay(50);
            break;
        case _101MS:
            delay(101);
            break;
        case _154MS:
            delay(154);
            break;
        case _700MS:
            delay(700);
            break;
    }
}

uint16_t TCS34725::getClear(void)
{
    return clear;
}

uint16_t TCS34725::getRed(void)
{
    return red;
}

uint16_t TCS34725::getGreen(void)
{
    return green;
}

uint16_t TCS34725::getBlue(void)
{
    return blue;
}

uint32_t TCS34725::getRGB888(void)
{
    uint16_t r = red;
    uint16_t g = green;
    uint16_t b = blue;

    float i = 1;

    if (r >= g && r >= b)
    {
        i = r / 255 + 1;
    }
    else if (g >= r && g >= b)
    {
        i = g / 255 + 1;
    }
    else if (b >= g && b >= r)
    {
        i = b / 255 + 1;
    }

    if (i != 0)
    {
        r /= i;
        g /= i;
        b /= i;
    }

    if (r > 30)
    {
        r -= 30;
    }
    if (g > 30)
    {
        g -= 30;
    }
    if (b > 30)
    {
        b -= 30;
    }

    r = r * 255 / 225;
    g = g * 255 / 225;
    b = b * 255 / 225;

    if (r > 255)
    {
        r = 255;
    }
    if (g > 255)
    {
        g = 255;
    }
    if (b > 255)
    {
        b = 255;
    }

    return (r << 16) | (g << 8) | (b);
}

// Converts the raw R/G/B values to color temperature in degrees Kelvin
// @return la temperature de couleur en degres Kelvin
uint16_t TCS34725::calculateColorTemperature(void)
{
    float X, Y, Z; /* RGB to XYZ correlation      */
    float xc, yc;  /* Chromaticity co-ordinates   */
    float n;       /* McCamy's formula            */
    float cct;

    /* 1. Map RGB values to their XYZ counterparts.    */
    /* Based on 6500K fluorescent, 3000K fluorescent   */
    /* and 60W incandescent values for a wide range.   */
    /* Note: Y = Illuminance or lux                    */
    X = (-0.14282F * red) + (1.54924F * green) + (-0.95641F * blue);
    Y = (-0.32466F * red) + (1.57837F * green) + (-0.73191F * blue);
    Z = (-0.68202F * red) + (0.77073F * green) + (0.56332F * blue);

    /* 2. Calculate the chromaticity co-ordinates      */
    xc = (X) / (X + Y + Z);
    yc = (Y) / (X + Y + Z);

    /* 3. Use McCamy's formula to determine the CCT    */
    n = (xc - 0.3320F) / (0.1858F - yc);

    /* Calculate the final CCT */
    cct = (449.0F * pow((double)n, 3.0)) + (3525.0F * pow((double)n, 2.0)) + (6823.3F * n) + 5520.33F;

    /* Return the results in degrees Kelvin */
    return (uint16_t)cct;
}

// Converts the raw R/G/B values to lux
// @Return la luminosite en lux
uint16_t TCS34725::calculateLux(void)
{
    float illuminance;

    /* This only uses RGB ... how can we integrate clear or calculate lux */
    /* based exclusively on clear since this might be more reliable?      */
    illuminance = (-0.32466F * red) + (1.57837F * green) + (-0.73191F * blue);

    return (uint16_t)illuminance;
}

// Initialise la broche d'interruption
// @Param initialisation oui ou non
void TCS34725::setInterrupt(bool i)
{
    uint8_t r = read_byte(ENABLE);
    if (i)
    {
        r |= AIEN;
    }
    else
    {
        r &= ~AIEN;
    }
    write_byte(ENABLE, r);
}

// Remet la broche INT a 0 pour aquitement et preparer l'interruption suivante
void TCS34725::clearInterrupt(void)
{
    write_byte(COMMAND_BIT | 0x66);
}

// Initialise les bornes de luminosite pour l'interruption
// Permet d'allumer la LED si la luminosite est trop faible
// (La broche INT doit etre connecte a la broche LED)
// @Param luminosite minimum
// @Param luminosite maximum
void TCS34725::setIntLimits(uint16_t low, uint16_t high)
{
    write_byte(AILTL, low & 0xFF);
    write_byte(AILTH, low >> 8);
    write_byte(AIHTL, high & 0xFF);
    write_byte(AIHTH, high >> 8);
}

TCS34725::~TCS34725()
{
    disable();
}
