#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "../Config/Config.h"

class DeviceManager;
class Access;
class Fingerprint;
class NFID;

class MessageRouter
{
public:

    MessageRouter(
        ConfigurationManager& configuration,
        DeviceManager& deviceManager,
        Access& access,
        Fingerprint& fingerprint,
        NFID& nfid);

    void Route(
        const String& topic,
        const String& payload);

private:

    ConfigurationManager& _configuration;

    DeviceManager& _deviceManager;

    Access& _access;

    Fingerprint& _fingerprint;

    NFID& _nfid;

    void HandleAuthentication(
        const JsonDocument& doc);

    void HandleSync(
        const JsonDocument& doc);

    void HandleCommand(
        const JsonDocument& doc);

    void HandleHeartbeat(
        const JsonDocument& doc);

    void HandleFingerprint(
        const JsonDocument& doc);

    void HandleRFID(
        const JsonDocument& doc);

    void HandleFingerprintRequest(
        const JsonDocument& doc);

    void HandleNFIDWrite(
        const JsonDocument& doc);
};