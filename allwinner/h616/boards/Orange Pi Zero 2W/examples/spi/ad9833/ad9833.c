#include "ad9833.h"
#include <wiringPi.h>

#define POW_2_28               0x10000000UL  // 2^28
#define CHANGE_PER_DEG         11.37         // 4096/360
#define CLAMP(min, value, max) ((value < min) ? (min) : ((value > max) ? max : value))

#define ad9833_delay_ms(t)     delay(t)

void ad9833_write(uint16_t data)
{
    AD9833_CS_LOW();
    wiringPiSPIDataRW(AD9833_SPI_CH, (uint8_t*)&data, 2);
    AD9833_CS_HIGH();
}

void ad9833_init(void)
{

    wiringPiSPISetupMode(AD9833_SPI_CH, AD9833_SPI_CS, 1e5, 3);

    ad9833_reset();

    ad9833_set_frequency(AD9833_FREQ0_SELECT, 0);
    ad9833_set_frequency(AD9833_FREQ1_SELECT, 0);
    ad9833_set_phase(AD9833_PHASE0_SELECT, 0);
    ad9833_set_phase(AD9833_PHASE1_SELECT, 0);
    ad9833_write(0x00);  // clear reset bit
    ad9833_write(AD9833_CMD_SLEEP);
}

void ad9833_reset(void)
{
    ad9833_write(AD9833_CMD_RESET);
    ad9833_delay_ms(10);  // 1ms
}

void ad9833_set_waveform(ad9833_waveform wave)
{
    ad9833_write(wave);
}

void ad9833_set_frequency(ad9833_freq_select cmd, float freq /*hz*/)
{
    uint32_t v = (float)freq * POW_2_28 / FMCLK;
    ad9833_write(cmd | (v & 0x3FFF));          // lower 14 bits
    ad9833_write(cmd | ((v >> 14) & 0x3FFF));  // upper 14 bits
}

void ad9833_set_phase(ad9833_phase_select cmd, float phase)
{
    phase      = CLAMP(0, phase, 360);
    uint16_t v = phase * CHANGE_PER_DEG;  // [0,4095]
    ad9833_write(cmd | v);
}

float ad9833_get_resolution(void)
{
    return (float)FMCLK / POW_2_28;
}

void ad9833_set_output(ad9833_freq_output freq, ad9833_phase_output phase)
{
    ad9833_write(freq);
    ad9833_write(phase);
}
