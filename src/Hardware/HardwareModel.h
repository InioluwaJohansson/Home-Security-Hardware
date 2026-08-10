#pragma once

#include <Arduino.h>
#include "Pins.h"
#include "../Facilities/FacilityLimits.h"
#include "../Facilities/DeviceModels.h"

struct DigitalEndpoint
{
    uint8_t chip;
    uint8_t pin;

    DigitalEndpoint(uint8_t chipValue = INVALID_CHANNEL,
                    uint8_t pinValue = INVALID_CHANNEL)
        : chip(chipValue), pin(pinValue) {}
};

struct PwmEndpoint
{
    uint8_t controller;
    uint8_t channel;

    PwmEndpoint(uint8_t controllerValue = INVALID_CHANNEL,
                uint8_t channelValue = INVALID_CHANNEL)
        : controller(controllerValue), channel(channelValue) {}
};

enum class SensorBus : uint8_t
{
    None = 0,
    DoorMux,
    WindowMux,
    ExternalMux,
    // channel is an actual ESP32 GPIO number.
    McuGpio,
    // channel is an MCP23017 pin; chip identifies the MCP device.
    McpGpio
};

struct SensorEndpoint
{
    SensorBus bus;
    uint8_t channel;
    bool activeHigh;
    uint8_t chip;

    SensorEndpoint(
        SensorBus busValue = SensorBus::None,
        uint8_t channelValue = INVALID_CHANNEL,
        bool activeHighValue = true,
        uint8_t chipValue = 0)
        : bus(busValue),
          channel(channelValue),
          activeHigh(activeHighValue),
          chip(chipValue) {}
};

struct CurrentSensorEndpoint
{
    uint8_t adc;
    uint8_t channel;

    // Voltage produced by the sensor at zero current.
    float zeroVoltage;

    // Sensor sensitivity in volts per ampere.
    float voltsPerAmp;

    CurrentSensorEndpoint(
        uint8_t adcValue = INVALID_CHANNEL,
        uint8_t channelValue = INVALID_CHANNEL,
        float zeroVoltageValue = 2.5f,
        float voltsPerAmpValue = 0.040f)
        : adc(adcValue),
          channel(channelValue),
          zeroVoltage(zeroVoltageValue),
          voltsPerAmp(voltsPerAmpValue) {}
};

struct MotorResource
{
    PwmEndpoint pwm;
    DigitalEndpoint direction;

    // Mechanical installation orientation.
    bool openDirection = true;

    // Encoder inputs are kept off the 74HC4067 buses. They are reserved for
    // position feedback and are not used as the primary safety stop. Hall
    // end-stops, current limits, and travel time remain the safety limits.
    SensorEndpoint encoderA;
    SensorEndpoint encoderB;
    SensorEndpoint openSensor;
    SensorEndpoint closeSensor;

    CurrentSensorEndpoint current;

    uint16_t maxSpeed = 100;
    uint16_t acceleration = 25;
    uint16_t deceleration = 25;
    uint16_t maxCurrent = 5000;
    uint32_t maxTravelTime = 30000;
};

struct LockResource
{
    PwmEndpoint pwm;
    DigitalEndpoint direction;

    // Mechanical direction that physically locks the facility.
    bool lockDirection = true;

    SensorEndpoint positionSensor;
    CurrentSensorEndpoint current;

    uint16_t maxCurrent = 2000;
    uint32_t maxTravelTime = 5000;
};

struct MotorizedFacilityHardware
{
    MotorResource motor;
    LockResource lock;
};

struct HardwareDevice
{
    int id = -1;
    FacilityType type = FacilityType::Light;
    bool enabled = false;

    // Backend-selected mechanical profile. Only doors use this field;
    // windows use the moderate-speed window profile.
    DoorType doorType = DoorType::Interior;

    // Simple facility resources.
    DigitalEndpoint primary;
    PwmEndpoint pwm;
    SensorEndpoint sensor;

    // Motorized facility resources.
    MotorizedFacilityHardware motorized;
};
