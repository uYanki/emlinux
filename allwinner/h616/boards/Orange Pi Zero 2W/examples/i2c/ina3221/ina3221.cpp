#include "ina3221.h"

// *****************
// Constructeur vide
// *****************
ina3221::ina3221()
{
}

// ******************************
// Constructeur
// @Param l'adresse I2C du module
// ******************************
ina3221::ina3221(Address address)
{
    init(address);
}

// *******************************
// Initialisation
// @Param l'adresse I2C du module
// @Return true si ok, false sinon
// *******************************
bool ina3221::init(Address address)
{
    I2C::init(address);
    return I2C::isInitialize();
}

// *****************************************************
// Configuration du circuit
// @Param channel1 actif
// @Param channel2 actif
// @Param channel3 actif
// @Param nombre de mesures pour moyennage
// @Param Temps de convertion de mesure du Bus Voltage
// @Param Temps de convertion de mesure du Shunt Voltage
// @Param Mode de mesure
// *****************************************************
bool ina3221::setConfiguration(Channel1 channel1, Channel2 channel2, Channel3 channel3, Average average, BusVoltageConversionTime busVoltageConversionTime, ShuntVoltageConversionTime shuntVoltageConversionTime, Mode mode)
{
    return !write_word_swapped(Config, channel1 | channel2 | channel3 | average | busVoltageConversionTime | shuntVoltageConversionTime | mode);
}

// *********************************
// Le channel 1 est t'il actif ?
// @Return true si oui, false si non
// *********************************
bool ina3221::isChannel1(void)
{
    return read_word_swapped(Config) & Channel1On;
}

// *********************************
// Le channel 2 est t'il actif ?
// @Return true si oui, false si non
// *********************************
bool ina3221::isChannel2(void)
{
    return read_word_swapped(Config) & Channel2On;
}

// *********************************
// Le channel 3 est t'il actif ?
// @Return true si oui, false si non
// *********************************
bool ina3221::isChannel3(void)
{
    return read_word_swapped(Config) & Channel3On;
}

// *******************************************
// Donne le nombre de mesures pour moyennage
// @Return le nombre de mesures pour moyennage
// *******************************************
Average ina3221::getAverage(void)
{
    return (Average)(read_word_swapped(Config) & Avg_1024);
}

// *********************************************
// Donne le temps de convertion du Bus Voltage
// @Return le temps de convertion du Bus Voltage
// *********************************************
BusVoltageConversionTime ina3221::getBusVoltageConversionTime(void)
{
    return (BusVoltageConversionTime)(read_word_swapped(Config) & VBUS_8244US);
}

// ***********************************************
// Donne le temps de convertion du Shunt Voltage
// @Return le temps de convertion du Shunt Voltage
// ***********************************************
ShuntVoltageConversionTime ina3221::getShuntVoltageConversionTime(void)
{
    return (ShuntVoltageConversionTime)(read_word_swapped(Config) & SBUS_8244US);
}

// *************************
// Donne le mode de mesure
// @Return le mode de mesure
// *************************
Mode ina3221::getMode(void)
{
    return (Mode)(read_word_swapped(Config) & ShuntAndBusVoltageContinuous);
}

// ***************
// Reset du module
// ***************
bool ina3221::reset(void)
{
    return !write_word_swapped(Config, RESET);
}

// *********************************************
// Donne la tension du Bus en Volt pour un canal
// @Param le canal
// @Return la tension en volt
// *********************************************
float ina3221::getBusVoltage_V(Channel channel)
{
    while (!isConversionOk())
        ;

    switch (channel)
    {
        case Channel_1:
            return ((read_word_swapped(Channel1BusVoltage) >> 3)) * _8MILLIVOLT;
        case Channel_2:
            return ((read_word_swapped(Channel2BusVoltage) >> 3)) * _8MILLIVOLT;
        case Channel_3:
            return ((read_word_swapped(Channel3BusVoltage) >> 3)) * _8MILLIVOLT;
    }

    return 0.f;
}

// **************************************************
// Donne la tension du Bus en milliVolt pour un canal
// @Param le canal
// @Return la tension en milliVolt
// **************************************************
float ina3221::getShuntVoltage_mV(Channel channel)
{
    while (!isConversionOk())
        ;

    switch (channel)
    {
        case Channel_1:
            return ((read_word_swapped(Channel1ShuntVoltage) >> 3)) * _40MICROVOLT;
        case Channel_2:
            return ((read_word_swapped(Channel2ShuntVoltage) >> 3)) * _40MICROVOLT;
        case Channel_3:
            return ((read_word_swapped(Channel3ShuntVoltage) >> 3)) * _40MICROVOLT;
    }

    return 0.f;
}

// ************************************
// Donne le courant en mA pour un canal
// @Param le canal
// @Return le courant en mA
// ************************************
float ina3221::getCurrent_mA(Channel channel)
{
    return getShuntVoltage_mV(channel) / ResistanceShunt;
}

// *************************************
// Donne la puissance en W pour un canal
// @Param le canal
// @Return la puissance en W
// *************************************
float ina3221::getPower_W(Channel channel)
{
    return getBusVoltage_V(channel) * getCurrent_mA(channel) / 1000.0;
}

// **********************************************
// Positionne la valeur shunt limite sur un canal
// @Param le canal
// @Param la valeur en mV
// **********************************************
bool ina3221::setCriticalShuntAlertLimit(Channel channel, float valeur)
{
    switch (channel)
    {
        case Channel_1:
            return !write_word_swapped(Channel1CriticalAlertLimit, (uint16_t)(valeur / _40MICROVOLT) << 3);
        case Channel_2:
            return !write_word_swapped(Channel2CriticalAlertLimit, (uint16_t)(valeur / _40MICROVOLT) << 3);
        case Channel_3:
            return !write_word_swapped(Channel3CriticalAlertLimit, (uint16_t)(valeur / _40MICROVOLT) << 3);
    }

    return false;
}

// **********************************************
// Positionne la valeur shunt alarme sur un canal
// @Param le canal
// @Param la valeur en mV
// **********************************************
bool ina3221::setWarningShuntAlertLimit(Channel channel, float valeur)
{
    switch (channel)
    {
        case Channel_1:
            return !write_word_swapped(Channel1WarningAlertLimit, (uint16_t)(valeur / _40MICROVOLT) << 3);
        case Channel_2:
            return !write_word_swapped(Channel2WarningAlertLimit, (uint16_t)(valeur / _40MICROVOLT) << 3);
        case Channel_3:
            return !write_word_swapped(Channel3WarningAlertLimit, (uint16_t)(valeur / _40MICROVOLT) << 3);
    }

    return false;
}

// *****************************************
// Donne la valeur shunt limite sur un canal
// @Param le canal
// @Return la valeur en mV
// *****************************************
float ina3221::getCriticalShuntAlertLimit(Channel channel)
{
    switch (channel)
    {
        case Channel_1:
            return ((read_word_swapped(Channel1CriticalAlertLimit) >> 3)) * _40MICROVOLT;
        case Channel_2:
            return ((read_word_swapped(Channel2CriticalAlertLimit) >> 3)) * _40MICROVOLT;
        case Channel_3:
            return ((read_word_swapped(Channel3CriticalAlertLimit) >> 3)) * _40MICROVOLT;
    }

    return 0.f;
}

// *****************************************
// Donne la valeur shunt alarme sur un canal
// @Param le canal
// @Return la valeur en mV
// *****************************************
float ina3221::getWarningShuntAlertLimit(Channel channel)
{
    switch (channel)
    {
        case Channel_1:
            return ((read_word_swapped(Channel1WarningAlertLimit) >> 3)) * _40MICROVOLT;
        case Channel_2:
            return ((read_word_swapped(Channel2WarningAlertLimit) >> 3)) * _40MICROVOLT;
        case Channel_3:
            return ((read_word_swapped(Channel3WarningAlertLimit) >> 3)) * _40MICROVOLT;
    }

    return 0.f;
}

// ******************************************
// Donne la somme des tensions sur les shunts
// @Return la somme des tensions en mV
// ******************************************
float ina3221::getShuntVoltageSum_mV(void)
{
    while (!isConversionOk())
        ;
    return ((read_word_swapped(ShuntVoltageSum) >> 1)) * _40MICROVOLT;
}

// *********************************************************
// Positionne la valeur limite de tension sur tous les shunt
// @Param valeur en mV
// *********************************************************
bool ina3221::setShuntVoltageSumLimit(float valeur)
{
    return !write_word_swapped(ShuntVoltageSumLimit, (uint16_t)(valeur / 0.04) << 1);
}

// ****************************************************
// Donne la valeur limite de tension sur tous les shunt
// @Return valeur en mV
// ****************************************************
float ina3221::getShuntVoltageSumLimit(void)
{
    return ((read_word_swapped(ShuntVoltageSumLimit) >> 1)) * _40MICROVOLT;
}

// ******************************************
// Donne la somme des courants sur les shunts
// @Return La somme des courants en mA
// ******************************************
float ina3221::getCurrentSum_mA(void)
{
    return getShuntVoltageSum_mV() / ResistanceShunt;
}

// ***************************************************************
// Positionne les mask
// @Param Active le canal1 pour la sommation des tensions de shunt
// @Param Active le canal2 pour la sommation des tensions de shunt
// @Param Active le canal3 pour la sommation des tensions de shunt
// @Param Active la Pin de warning
// @Param Active la pin d'alerte critique
// ***************************************************************
bool ina3221::setMaskEnable(SummationChannelControl1 summationChannelControl1, SummationChannelControl2 summationChannelControl2, SummationChannelControl3 summationChannelControl3, WarningAlertLatchEnable warningAlertLatchEnable, CriticalAlertLatchEnable criticalAlertLatchEnable)
{
    return !write_word_swapped(MaskEnable, summationChannelControl1 | summationChannelControl2 | summationChannelControl3 | warningAlertLatchEnable | criticalAlertLatchEnable);
}

// *************************************************************
// Le canal1 participe t'il à la sommation des tensions de shunt
// @Return true si oui, false si non
// *************************************************************
bool ina3221::isSummationChannelControl1(void)
{
    return read_word_swapped(MaskEnable) & SummationChannelControl1On;
}

// *************************************************************
// Le canal2 participe t'il à la sommation des tensions de shunt
// @Return true si oui, false si non
// *************************************************************
bool ina3221::isSummationChannelControl2(void)
{
    return read_word_swapped(MaskEnable) & SummationChannelControl2On;
}

// *************************************************************
// Le canal3 participe t'il à la sommation des tensions de shunt
// @Return true si oui, false si non
// *************************************************************
bool ina3221::isSummationChannelControl3(void)
{
    return read_word_swapped(MaskEnable) & SummationChannelControl3On;
}

// ******************************************
// Le WarningAlertLatchEnable est t'il actif
// @Return true si oui, false si non
// ******************************************
bool ina3221::isWarningAlertLatchEnable(void)
{
    return read_word_swapped(MaskEnable) & WarningAlertLatchEnableOn;
}

// ******************************************
// Le CriticalAlertLatchEnable est t'il actif
// @Return true si oui, false si non
// ******************************************
bool ina3221::isCriticalAlertLatchEnable(void)
{
    return read_word_swapped(MaskEnable) & CriticalAlertLatchEnableOn;
}

// ********************************************
// Le canal1 a t'il active le CriticalAlertFlag
// @Return true si oui, false si non
// ********************************************
bool ina3221::isCriticalAlertFlagIndicator1(void)
{
    return read_word_swapped(MaskEnable) & CriticalAlertFlagIndicator1On;
}

// ********************************************
// Le canal2 a t'il active le CriticalAlertFlag
// @Return true si oui, false si non
// ********************************************
bool ina3221::isCriticalAlertFlagIndicator2(void)
{
    return read_word_swapped(MaskEnable) & CriticalAlertFlagIndicator2On;
}

// ********************************************
// Le canal3 a t'il active le CriticalAlertFlag
// @Return true si oui, false si non
// ********************************************
bool ina3221::isCriticalAlertFlagIndicator3(void)
{
    return read_word_swapped(MaskEnable) & CriticalAlertFlagIndicator3On;
}

// ************************************
// Le SummationAlertFlag est t'il actif
// @Return true si oui, false si non
// ************************************
bool ina3221::isSummationAlertFlag(void)
{
    return read_word_swapped(MaskEnable) & SummationAlertFlagOn;
}

// *******************************************
// Le canal1 a t'il active le WarningAlertFlag
// @Return true si oui, false si non
// *******************************************
bool ina3221::isWarningAlertFlagIndicator1(void)
{
    return read_word_swapped(MaskEnable) & WarningAlertFlagIndicator1On;
}

// *******************************************
// Le canal2 a t'il active le WarningAlertFlag
// @Return true si oui, false si non
// *******************************************
bool ina3221::isWarningAlertFlagIndicator2(void)
{
    return read_word_swapped(MaskEnable) & WarningAlertFlagIndicator2On;
}

// *******************************************
// Le canal3 a t'il active le WarningAlertFlag
// @Return true si oui, false si non
// *******************************************
bool ina3221::isWarningAlertFlagIndicator3(void)
{
    return read_word_swapped(MaskEnable) & WarningAlertFlagIndicator3On;
}

// ************************************************************************************
// Lit le PowerValidAlertFlag
// @Return true si toutes les tensions de bus sont au dessus de la powerValidUpperLimit
//         false si une des tensions de bus est au dessous de la powerValidLowerLimit
// ************************************************************************************
bool ina3221::isPowerValidAlertFlag(void)
{
    return read_word_swapped(MaskEnable) & PowerValidAlertFlagOn;
}

// ****************************************
// Le TimingControlAlertFlag est t'il actif
// @Return false si ok, true si non
// ****************************************
bool ina3221::isTimingControlAlertFlag(void)
{
    return !(read_word_swapped(MaskEnable) & TimingControlAlertFlagOn);
}

// ******************************************
// La premiere conversion est t'elle en cours
// @Return true si oui, false si non
// ******************************************
bool ina3221::isConversionOk(void)
{
    return read_word_swapped(MaskEnable) & ConversionWriting;
}

// *************************************************************************************************
// Positionne la valeur de comparaison haute des tensions de bus pour activer le PowerValidAlertFlag
// @Param valeur de comparaison haute des tensions de bus
// *************************************************************************************************
bool ina3221::setPowerValidUpperLimit(float valeur)
{
    return !write_word_swapped(PowerValidUpperLimit, (uint16_t)(valeur / _8MILLIVOLT) << 3);
}

// ******************************************************************************************************
// Positionne la valeur de comparaison basse de chaque tension de bus pour activer le PowerValidAlertFlag
// @Param valeur de comparaison basse des tensions de bus
// ******************************************************************************************************
bool ina3221::setPowerValidLowerLimit(float valeur)
{
    return !write_word_swapped(PowerValidLowerLimit, (uint16_t)(valeur / _8MILLIVOLT) << 3);
}

// ************************************************************
// Donne la valeur de comparaison haute des tensions de bus
// @Return valeur de comparaison haute des tensions de bus en V
// ************************************************************
float ina3221::getPowerValidUpperLimit(void)
{
    return ((read_word_swapped(PowerValidUpperLimit) >> 3)) * _8MILLIVOLT;
}

// ************************************************************
// Donne la valeur de comparaison basse des tensions de bus
// @Return valeur de comparaison basse des tensions de bus en V
// ************************************************************
float ina3221::getPowerValidLowerLimit(void)
{
    return ((read_word_swapped(PowerValidLowerLimit) >> 3)) * _8MILLIVOLT;
}

// **********************************************************
// Donne le Manufacturer ID (TI pour Texas Instrument 0x5449)
// @Return le manufacturer ID
// **********************************************************
uint16_t ina3221::getManufacturerID(void)
{
    return read_word_swapped(ManufacturerID);
}

// ***************************************
// Le Manufacturer ID est t'il TI (0x5449)
// @Return true si oui, false si non
// ***************************************
bool ina3221::isManufacturerID(void)
{
    return !(getManufacturerID() & ~TI);
}

// *********************************
// Donne l'ID de l'appareil (0x3220)
// @Return l'ID de l'appareil
// *********************************
uint16_t ina3221::getDieID(void)
{
    return read_word_swapped(DieID);
}

// **********************************
// L'ID de l'appareil est t'il 0x3220
// @Return true si oui, false si non
// **********************************
bool ina3221::isDieID(void)
{
    return !(getDieID() & ~INA3221);
}
