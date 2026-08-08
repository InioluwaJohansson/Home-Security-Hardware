#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include "INFIDProtocol.h"

class PN532Protocol : public INFIDProtocol
{
public:
    explicit PN532Protocol(TwoWire& wire);

    bool Initialize() override;
    bool CardPresent() override;
    bool ReadCode(String& code) override;
    bool WriteCode(const String& code) override;

private:
    TwoWire& _wire;
    Adafruit_PN532* _nfc = nullptr;

    uint8_t _uid[7] = {};
    uint8_t _uidLength = 0;

    bool ReadPage(uint8_t page, uint8_t data[4]);
    bool WritePage(uint8_t page, const uint8_t data[4]);
    bool ReadPages(uint8_t* buffer, uint16_t& length);
    bool WritePages(const uint8_t* data, uint16_t length);
    bool ClearUserMemory();
    bool ReadSignature();
    bool WriteSignature();

    static constexpr uint8_t SignaturePage = 4;
    static constexpr char Signature[] = "Home";

    static constexpr uint8_t FirstUserPage = 5;
    static constexpr uint8_t LastUserPage = 39;
    static constexpr uint16_t UserMemorySize =
        (LastUserPage - FirstUserPage + 1) * 4;
};
