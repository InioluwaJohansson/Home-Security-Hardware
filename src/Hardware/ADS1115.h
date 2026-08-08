#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

class ADS1115Controller
{
public:
    static constexpr uint8_t ADC_COUNT = 4;

    bool Initialize();
    float ReadVoltage(uint8_t adc, uint8_t channel);
    float ReadCurrent(uint8_t adc, uint8_t channel, float zeroVoltage, float voltsPerAmp);

private:
    static constexpr uint8_t Addresses[ADC_COUNT] = { 0x48, 0x49, 0x4A, 0x4B };
    Adafruit_ADS1115 _adcs[ADC_COUNT];
};
