#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>
#include "IFingerprintProtocol.h"
#include "R50xSystemParameters.h"

class R50xProtocol : public IFingerprintProtocol {
public:
    explicit R50xProtocol(HardwareSerial& serial);
    ~R50xProtocol() override;
    bool Initialize() override;
    bool IsFingerDetected() override;
    bool CaptureImage() override;
    bool UploadImage(uint8_t*& image, uint32_t& length) override;
    void Cancel() override;
    const R50xSystemParameters& GetSystemParameters() const;
private:
    bool VerifyPassword();
    bool ReadSystemParameters();
    bool GenerateImage();
    bool ReceiveImagePackets(uint8_t*& image, uint32_t& imageLength);
    HardwareSerial& _serial;
    uint8_t* _imageBuffer = nullptr;
    uint32_t _imageLength = 0;
    R50xSystemParameters _systemParameters;
    static constexpr uint32_t MaxImageSize = 40000;
};
