#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

#include "IFingerprintProtocol.h"
#include "R50xSystemParameters.h"

class R30xProtocol : public IFingerprintProtocol
{
public:

    explicit R30xProtocol(
        HardwareSerial& serial);

    bool Initialize() override;

    bool IsFingerDetected() override;

    bool CaptureImage() override;

    bool UploadImage(
        uint8_t*& image,
        uint32_t& length) override;

    void Cancel() override;
    const R30xSystemParameters&
GetSystemParameters() const;

private:

    //----------------------------------------------------
    // Low Level Protocol
    //----------------------------------------------------

    bool VerifyPassword();

    bool ReadSystemParameters();

    bool SendPacket(
        uint8_t instruction,
        const uint8_t* payload = nullptr,
        uint16_t payloadLength = 0);

    bool ReceivePacket(
        uint8_t* buffer,
        uint32_t& length,
        uint32_t timeout = 3000);

    bool ReceiveImagePackets(
        uint8_t*& image,
        uint32_t& imageLength);

    bool WaitForAck(
        uint8_t expectedInstruction,
        uint32_t timeout = 3000);

    uint16_t CalculateChecksum(
        const uint8_t* data,
        uint16_t length);

private:

    HardwareSerial& _serial;

    uint8_t* _imageBuffer = nullptr;

    uint32_t _imageLength = 0;
private:

    R30xSystemParameters _systemParameters;
private:

    static constexpr uint32_t MaxImageSize = 40000;

    uint8_t* _imageBuffer = nullptr;

    uint32_t _imageLength = 0;
};