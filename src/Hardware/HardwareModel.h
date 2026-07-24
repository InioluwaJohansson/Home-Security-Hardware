#pragma once

#include <Arduino.h>
#include "../Facilities/Devices.h"
#include "../Facilities/DeviceModels.h"
constexpr uint8_t MAX_LIGHTS      = 6;
constexpr uint8_t MAX_APPLIANCES  = 8;
constexpr uint8_t MAX_DOORS       = 4;
constexpr uint8_t MAX_WINDOWS     = 4;
constexpr uint8_t MAX_CAMERAS     = 3;
constexpr uint8_t MAX_EXTERNALS   = 5;


constexpr uint8_t INVALID_CHANNEL = 255;

struct HardwareEndpoint
{
    uint8_t Chip = INVALID_CHANNEL;

    uint8_t Pin = INVALID_CHANNEL;

    uint8_t PwmChannel = INVALID_CHANNEL;

    uint8_t MultiplexerChannel = INVALID_CHANNEL;
};

struct HardwareDevice
{
    int Id = -1;

    FacilityType Type;

    bool Enabled = false;

    bool SupportsPWM = false;

    HardwareEndpoint Primary;

    HardwareEndpoint Secondary;
    
    HardwareEndpoint Sensor;
};