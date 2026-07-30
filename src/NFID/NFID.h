#pragma once

#include <Arduino.h>
#include <functional>

#include "PN532Protocol.h"
#include "../Models/NFIDModels.h"

class ConfigurationManager;
class MqttManager;

class NFID
{
public:

    NFID(
        ConfigurationManager& configuration,
        MqttManager& mqtt,
        uint8_t sda,
        uint8_t scl);

    bool Initialize();

    void Update();

    void QueueWrite(
        const String& nfidCode);

private:

    void HandleRead();

    void HandleWrite();

    void PublishVerification(
        const String& nfidCode);

private:

    Configuration& _configuration;

    MqttManager& _mqtt;

    PN532Protocol _protocol;

    NFIDContext _context;
};