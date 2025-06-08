
#include <wiringPiI2C.h>
#include "i2cbus.h"

// *****************
// Constructeur vide
// *****************
I2C::I2C()
{
}

// *************************
// Constructeur
// @param adresse du circuit
// *************************
I2C::I2C(uint16_t pAddress)
{
    init(pAddress);
}

// ******************************************
// Initialisation si constructeur vide appele
// @param adresse du circuit
// ******************************************
void I2C::init(uint16_t pAddress)
{
    handle     = wiringPiI2CSetup(pAddress);
    initialize = true;
    if (handle == -1)
    {
        initialize = false;
    }
}

// ********************************
// L'I2C est t'il initialise
// @return true si oui false si non
// ********************************
bool I2C::isInitialize(void)
{
    return initialize;
}
// **************************************************
// Lecture d'une valeur dans un circuit sans registre
// @return valeur lue si positif, erreur si negatif
// **************************************************
int8_t I2C::read_byte(void)
{
    return wiringPiI2CRead(handle);
}

// ***************************************************
// Ecriture d'une valeur dans un circuit sans registre
// @param la valeur a ecrire
// @return true si ok, false si erreur
// ***************************************************
bool I2C::write_byte(uint8_t data)
{
    if (wiringPiI2CWrite(handle, data))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// ***************************************************
// Lecture d'une valeur dans un circuit avec registres
// @param le registre
// @return valeur lue si positif, erreur si negatif
// ***************************************************
int8_t I2C::read_byte(uint8_t reg)
{
    return wiringPiI2CReadReg8(handle, reg);
}

// ****************************************************
// Ecriture d'une valeur dans un circuit avec registres
// @param le registre
// @param la valeur a ecrire
// @return true si ok, false si erreur
// ****************************************************
bool I2C::write_byte(uint8_t reg, uint8_t data)
{
    return wiringPiI2CWriteReg8(handle, reg, data) ? true : false;
}

// ***************************************************
// Lecture d'une valeur dans un circuit avec registres
// @param le registre
// @return valeur lue si positif, erreur si negatif
// ***************************************************
int16_t I2C::read_word(uint8_t reg)
{
    return wiringPiI2CReadReg16(handle, reg);
}

// ****************************************************
// Ecriture d'une valeur dans un circuit avec registres
// @param le registre
// @param la valeur a ecrire
// @return true si ok, false si erreur
// ****************************************************
bool I2C::write_word(uint8_t reg, uint16_t data)
{
    return wiringPiI2CWriteReg16(handle, reg, data) ? true : false;
}

// ***************************************************
// Lecture d'une valeur dans un circuit avec registres
// Les deux octets sont swappe apres lecture
// @param le registre
// @return valeur lue si positif, erreur si negatif
// ***************************************************
int16_t I2C::read_word_swapped(uint8_t reg)
{
    i2cData data;
    data.uSData = wiringPiI2CReadReg16(handle, reg);
    swap(data);
    return data.uSData;
}

// ****************************************************
// Ecriture d'une valeur dans un circuit avec registres
// Les deux octets sont swappe avant l'ecriture
// @param le registre
// @param la valeur a ecrire
// @return true si ok, false si erreur
// ****************************************************
bool I2C::write_word_swapped(uint8_t reg, uint16_t val)
{
    i2cUData data;
    data.uSData = val;
    swap(data);
    return wiringPiI2CWriteReg16(handle, reg, data.uSData) ? true : false;
}

// *********************************
// swap les deux octets d'un i2cData
// *********************************
void I2C::swap(i2cData& data)
{
    int8_t temp    = data.uCData[0];
    data.uCData[0] = data.uCData[1];
    data.uCData[1] = temp;
}

// **********************************
// swap les deux octets d'un i2cUData
// **********************************
void I2C::swap(i2cUData& data)
{
    uint8_t temp   = data.uCData[0];
    data.uCData[0] = data.uCData[1];
    data.uCData[1] = temp;
}

// ***********
// Destructeur
// ***********
I2C::~I2C()
{
}
