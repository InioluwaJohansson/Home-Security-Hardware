#include "PCA9685.h"

bool PCA9685Controller::Initialize()
{
    for (uint8_t i = 0; i < CONTROLLER_COUNT; ++i)
    {
        if (!_drivers[i].begin()) return false;
        _drivers[i].setPWMFreq(1000);
        for (uint8_t channel = 0; channel < 16; ++channel)
            _drivers[i].setPWM(channel, 0, 0);
    }
    return true;
}

void PCA9685Controller::SetDuty(uint8_t controller, uint8_t channel, uint16_t duty)
{
    if (controller >= CONTROLLER_COUNT || channel >= 16) return;
    if (duty > 4095) duty = 4095;
    _drivers[controller].setPWM(channel, 0, duty);
}

void PCA9685Controller::SetPercent(uint8_t controller, uint8_t channel, uint8_t percent)
{
    if (percent > 100) percent = 100;
    SetDuty(controller, channel, static_cast<uint16_t>((4095UL * percent) / 100UL));
}

void PCA9685Controller::TurnOff(uint8_t controller, uint8_t channel)
{
    SetDuty(controller, channel, 0);
}

constexpr uint8_t PCA9685Controller::Addresses[PCA9685Controller::CONTROLLER_COUNT];
