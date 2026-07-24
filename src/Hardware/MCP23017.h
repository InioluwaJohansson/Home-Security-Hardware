#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

class MCP23017Controller
{
public:

    bool Initialize();

    void DigitalWrite(
        uint8_t chip,
        uint8_t pin,
        bool state);

    bool DigitalRead(
        uint8_t chip,
        uint8_t pin);

    void PinMode(
        uint8_t chip,
        uint8_t pin,
        uint8_t mode);

private:

    static constexpr uint8_t CHIP_COUNT = 3;

    static constexpr uint8_t Addresses[CHIP_COUNT] =
    {
        0x20,
        0x21,
        0x22
    };

    Adafruit_MCP23X17 _chips[CHIP_COUNT];
};