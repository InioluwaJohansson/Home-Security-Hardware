#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

#include "../MQTT/MQTT.h"
#include "../Config/Config.h"

class Fingerprint
{
public:

    Fingerprint(
        HardwareSerial& serial,
        MqttManager& mqtt,
        ConfigurationManager& configuration);

    bool Initialize();

    void Update();

    //--------------------------------------------------
    // Backend Commands
    //--------------------------------------------------

    void RequestCapture(int personId);

    void RequestVerification();

private:

    enum class Mode
    {
        Idle,
        Capture,
        Verify
    };

    HardwareSerial& _serial;

    MqttManager& _mqtt;

    ConfigurationManager& _configuration;

    Mode _mode = Mode::Idle;

    int _personId = -1;

    //--------------------------------------------------
    // R503 Protocol
    //--------------------------------------------------

    bool GetImage();

    bool UploadImage();

    bool ReadPacket(
        uint8_t* buffer,
        uint32_t& length,
        uint32_t timeout = 3000);

    bool SendPacket(
        uint16_t command,
        const uint8_t* data = nullptr,
        uint16_t length = 0);

    uint16_t CalculateChecksum(
        const uint8_t* data,
        uint16_t length);

    bool WaitForAck(
        uint16_t expectedCommand,
        uint32_t timeout = 3000);

    //--------------------------------------------------
    // Operations
    //--------------------------------------------------

    void CaptureFingerprint();

    void VerifyFingerprint();

    void PublishCapture(
        const uint8_t* image,
        uint32_t length);

    void PublishVerification(
        const uint8_t* image,
        uint32_t length);
};