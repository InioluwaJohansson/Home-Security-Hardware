#pragma once

#include <Arduino.h>

struct R50xSystemParameters
{
    //--------------------------------------------------
    // System Configuration
    //--------------------------------------------------

    uint16_t StatusRegister = 0;

    uint16_t SystemIdentifier = 0;

    uint16_t LibraryCapacity = 0;

    uint16_t SecurityLevel = 0;

    uint32_t DeviceAddress = 0;

    uint16_t PacketLength = 32;

    uint16_t PacketLengthCode = 0;

    uint32_t BaudRate = 57600;
};