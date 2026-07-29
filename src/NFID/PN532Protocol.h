#pragma once

#include <Arduino.h>

#include <Wire.h>

#include <Adafruit_PN532.h>

#include "INFIDProtocol.h"

class PN532Protocol :
    public INFIDProtocol
{
public:

    PN532Protocol(
        uint8_t sda,
        uint8_t scl);

    bool Initialize() override;

    bool CardPresent() override;

    bool ReadCode(
        String& code) override;

    bool WriteCode(
        const String& code) override;

private:

    TwoWire _wire;

    Adafruit_PN532* _nfc = nullptr;
private:

    bool WaitForCard(
        uint32_t timeout = 30000);

    bool ReadPages(
        uint8_t* buffer,
        uint16_t& length);

    bool WritePages(
        const uint8_t* data,
        uint16_t length);

private:

    TwoWire _wire;

    Adafruit_PN532* _nfc = nullptr;

    uint8_t _uid[7];

    uint8_t _uidLength = 0;

    static constexpr uint8_t FirstUserPage = 4;

    static constexpr uint8_t LastUserPage = 39;
};