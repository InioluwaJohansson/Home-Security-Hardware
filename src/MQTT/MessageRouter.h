#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "../Config/Config.h"

class DeviceManager;

class Access;

class MessageRouter
{
public:

    MessageRouter(
        ConfigurationManager& configuration,
        DeviceManager& deviceManager,
        Access& access);

    void Route(
        const String& topic,
        const String& payload);

private:

    ConfigurationManager& _configuration;

    DeviceManager& _deviceManager;

    Access& _access;

    void HandleAuthentication(const JsonDocument& doc);

    void HandleSync(const JsonDocument& doc);

    void HandleCommand(const JsonDocument& doc);

    void HandleHeartbeat(const JsonDocument& doc);

    void HandleFingerprint(const JsonDocument& doc);

    void HandleRFID(const JsonDocument& doc);
};