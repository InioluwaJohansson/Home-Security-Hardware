#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "../Config/Config.h"
#include "../MQTT/MQTT.h"
#include "PN532Protocol.h"
#include "NFIDModels.h"

class NFID {
public:
    NFID(ConfigurationManager& configuration, MqttManager& mqtt, TwoWire& wire);
    NFID(ConfigurationManager& configuration, MqttManager& mqtt, uint8_t sda, uint8_t scl);
    bool Initialize();
    void Update();
    void QueueWrite(const String& nfidCode);
private:
    void HandleRead();
    void HandleWrite();
    void PublishVerification(const String& nfidCode);
    ConfigurationManager& _configuration;
    MqttManager& _mqtt;
    PN532Protocol _protocol;
    NFIDContext _context;
};
