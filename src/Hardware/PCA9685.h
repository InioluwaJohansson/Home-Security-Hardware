#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

class PCA9685Controller
{
public:

    bool Initialize();

    void SetBrightness(
        uint8_t channel,
        uint8_t brightness);

    void TurnOff(uint8_t channel);

    void TurnOn(uint8_t channel);

private:

    static constexpr uint8_t Address = 0x40;

    Adafruit_PWMServoDriver _driver =
        Adafruit_PWMServoDriver(Address);
};