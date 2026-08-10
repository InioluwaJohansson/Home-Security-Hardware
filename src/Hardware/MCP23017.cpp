#include "MCP23017.h"

bool MCP23017Controller::Initialize()
{
    for (uint8_t i = 0; i < CHIP_COUNT; ++i)
    {
        if (!_chips[i].begin_I2C(Addresses[i], &Wire))
            return false;

        for (uint8_t pin = 0; pin < 16; ++pin)
        {
            _chips[i].pinMode(pin, OUTPUT);
            _chips[i].digitalWrite(pin, LOW);
        }
    }

    // MCP #3 provides the eight window quadrature encoder inputs and spare encoder channels.
    for (uint8_t pin = 0; pin < 16; ++pin)
        _chips[CHIP_COUNT - 1].pinMode(pin, INPUT_PULLUP);

    return true;
}

void MCP23017Controller::PinMode(uint8_t chip, uint8_t pin, uint8_t mode)
{
    if (chip >= CHIP_COUNT || pin >= 16) return;
    _chips[chip].pinMode(pin, mode);
}

void MCP23017Controller::DigitalWrite(uint8_t chip, uint8_t pin, bool state)
{
    if (chip >= CHIP_COUNT || pin >= 16) return;
    _chips[chip].digitalWrite(pin, state ? HIGH : LOW);
}

bool MCP23017Controller::DigitalRead(uint8_t chip, uint8_t pin)
{
    if (chip >= CHIP_COUNT || pin >= 16) return false;
    return _chips[chip].digitalRead(pin);
}

constexpr uint8_t MCP23017Controller::Addresses[MCP23017Controller::CHIP_COUNT];
