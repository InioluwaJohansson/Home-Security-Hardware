#pragma once

#include <Arduino.h>
#include "../Facilities/Devices.h"
#include "../Facilities/FacilityLimits.h"
#include "../Facilities/DeviceModels.h"

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