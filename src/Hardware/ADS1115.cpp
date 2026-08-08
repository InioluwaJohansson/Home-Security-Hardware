#include "ADS1115.h"

bool ADS1115Controller::Initialize()
{
    for (uint8_t i = 0; i < ADC_COUNT; ++i)
    {
        if (!_adcs[i].begin(Addresses[i], &Wire)) return false;
        _adcs[i].setGain(GAIN_ONE);
        _adcs[i].setDataRate(RATE_ADS1115_860SPS);
    }
    return true;
}

float ADS1115Controller::ReadVoltage(uint8_t adc, uint8_t channel)
{
    if (adc >= ADC_COUNT || channel >= 4) return 0.0f;
    const int16_t raw = _adcs[adc].readADC_SingleEnded(channel);
    return _adcs[adc].computeVolts(raw);
}

float ADS1115Controller::ReadCurrent(uint8_t adc, uint8_t channel, float zeroVoltage, float voltsPerAmp)
{
    if (voltsPerAmp <= 0.0f) return 0.0f;
    return (ReadVoltage(adc, channel) - zeroVoltage) / voltsPerAmp;
}

constexpr uint8_t ADS1115Controller::Addresses[ADS1115Controller::ADC_COUNT];
