#include "MCP23017.h"

#include "Pins.h"

bool MCP23017Controller::Initialize()
{
    Wire.begin(Pins::SDA, Pins::SCL);

    for(uint8_t i = 0; i < CHIP_COUNT; i++)
    {
        if(!_chips[i].begin_I2C(Addresses[i]))
        {
            return false;
        }
    }

    return true;
}

void MCP23017Controller::PinMode(
    uint8_t chip,
    uint8_t pin,
    uint8_t mode)
{
    if(chip >= CHIP_COUNT)
        return;

    _chips[chip].pinMode(pin, mode);
}

void MCP23017Controller::DigitalWrite(
    uint8_t chip,
    uint8_t pin,
    bool state)
{
    if(chip >= CHIP_COUNT)
        return;

    _chips[chip].digitalWrite(pin, state);
}

bool MCP23017Controller::DigitalRead(
    uint8_t chip,
    uint8_t pin)
{
    if(chip >= CHIP_COUNT)
        return false;

    return _chips[chip].digitalRead(pin);
}