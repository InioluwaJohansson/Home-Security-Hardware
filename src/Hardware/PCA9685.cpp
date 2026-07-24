#include "PCA9685.h"

#include "Pins.h"

bool PCA9685Controller::Initialize()
{
    Wire.begin(Pins::SDA, Pins::SCL);

    _driver.begin();

    _driver.setPWMFreq(1000);

    for(uint8_t i = 0; i < 16; i++)
    {
        _driver.setPWM(i, 0, 0);
    }

    return true;
}

void PCA9685Controller::SetBrightness(
    uint8_t channel,
    uint8_t brightness)
{
    if(channel >= 16)
        return;

    uint16_t pwm =
        map(brightness,
            0,
            100,
            0,
            4095);

    _driver.setPWM(channel, 0, pwm);
}

void PCA9685Controller::TurnOn(uint8_t channel)
{
    SetBrightness(channel, 100);
}

void PCA9685Controller::TurnOff(uint8_t channel)
{
    SetBrightness(channel, 0);
}