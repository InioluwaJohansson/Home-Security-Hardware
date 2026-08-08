#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>
#include "../MQTT/MQTT.h"
#include "../Config/Config.h"
#include "FingerprintModels.h"
#include "R50xProtocol.h"

class Fingerprint {
public:
    Fingerprint(HardwareSerial& serial, MqttManager& mqtt, ConfigurationManager& configuration);
    bool Initialize();
    void Update();
    void RequestCapture(int personId);
    void RequestVerification();
    const FingerprintContext& GetContext() const { return _context; }
private:
    HardwareSerial& _serial;
    MqttManager& _mqtt;
    ConfigurationManager& _configuration;
    R50xProtocol _protocol;
    FingerprintContext _context;
    bool CaptureFingerprint();
    bool VerifyFingerprint();
    void SetFailed();
};
