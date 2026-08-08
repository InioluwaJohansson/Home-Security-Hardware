#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

class PCA9685Controller
{
public:
    static constexpr uint8_t CONTROLLER_COUNT = 2;

    bool Initialize();
    void SetDuty(uint8_t controller, uint8_t channel, uint16_t duty);
    void SetPercent(uint8_t controller, uint8_t channel, uint8_t percent);
    void TurnOff(uint8_t controller, uint8_t channel);

private:
    static constexpr uint8_t Addresses[CONTROLLER_COUNT] = { 0x40, 0x41 };
    Adafruit_PWMServoDriver _drivers[CONTROLLER_COUNT] = {
        Adafruit_PWMServoDriver(Addresses[0]),
        Adafruit_PWMServoDriver(Addresses[1])
    };
};
