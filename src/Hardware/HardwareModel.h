#pragma once

#include <Arduino.h>
#include "Pins.h"
#include "../Facilities/FacilityLimits.h"
#include "../Facilities/DeviceModels.h"

struct DigitalEndpoint
{
    uint8_t chip = INVALID_CHANNEL;
    uint8_t pin = INVALID_CHANNEL;
};

struct PwmEndpoint
{
    uint8_t controller = INVALID_CHANNEL;
    uint8_t channel = INVALID_CHANNEL;
};

enum class SensorBus : uint8_t
{
    None = 0,
    DoorMux,
    WindowMux,
    ExternalMux,
    McuGpio
};

struct SensorEndpoint
{
    SensorBus bus = SensorBus::None;
    uint8_t channel = INVALID_CHANNEL;
    bool activeHigh = true;
};

struct CurrentSensorEndpoint
{
    uint8_t adc = INVALID_CHANNEL;
    uint8_t channel = INVALID_CHANNEL;

    // Voltage produced by the sensor at zero current.
    float zeroVoltage = 2.5f;

    // Sensor sensitivity in volts per ampere.
    float voltsPerAmp = 0.040f;
};

struct MotorResource
{
    PwmEndpoint pwm;
    DigitalEndpoint direction;

    // Mechanical installation orientation.
    bool openDirection = true;

    // Encoder inputs are intentionally represented independently from the
    // 74HC4067 sensor buses. High-frequency encoder signals must not be
    // multiplexed because doing so would lose pulses.
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

    // Simple facility resources.
    DigitalEndpoint primary;
    PwmEndpoint pwm;
    SensorEndpoint sensor;

    // Motorized facility resources.
    MotorizedFacilityHardware motorized;
};
