#pragma once

#include <Arduino.h>

class INFIDProtocol
{
public:

    virtual ~INFIDProtocol() = default;

    virtual bool Initialize() = 0;

    virtual bool CardPresent() = 0;

    virtual bool ReadCode(
        String& code) = 0;

    virtual bool WriteCode(
        const String& code) = 0;
};