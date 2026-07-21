#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "../Config/Config.h"

class DeviceManager;

class MessageRouter
{
public:

    MessageRouter(
        ConfigurationManager& configuration,
        DeviceManager& deviceManager);

    void Route(
        const String& topic,
        const String& payload);

private:

    ConfigurationManager& _configuration;

    DeviceManager& _deviceManager;

    void HandleAuthentication(const JsonDocument& doc);

    void HandleSync(const JsonDocument& doc);

    void HandleCommand(const JsonDocument& doc);

    void HandleHeartbeat(const JsonDocument& doc);

    void HandleFingerprint(const JsonDocument& doc);

    void HandleRFID(const JsonDocument& doc);
};