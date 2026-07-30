#pragma once

#include <Arduino.h>

class IFingerprintProtocol
{
public:

    virtual ~IFingerprintProtocol() = default;

    virtual bool Initialize() = 0;

    virtual bool IsFingerDetected() = 0;

    virtual bool CaptureImage() = 0;

    virtual bool UploadImage(
        uint8_t*& image,
        uint32_t& length) = 0;

    virtual void Cancel() = 0;
};