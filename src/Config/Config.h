#pragma once

#include <Arduino.h>
#include "../Storage/Storage.h"
#include "../Authentication/JWTAuth.h"

class ConfigurationManager
{
public:

    bool Initialize();

    Config& GetConfiguration();

    bool Save();

    void SetJwt(const String& token);

    const String& GetJwt() const;

    bool IsAuthenticated() const;

    void Logout();

    bool IsProvisioned() const;

private:

    StorageManager storage;
    JwtManager jwt;
    Config configuration;
};

struct Config
{
    String HardwareId;
    String AuthKey;

    String WifiSSID;
    String WifiPassword;

    String MqttHost;
    uint16_t MqttPort = 1883;
    String MqttUsername;
    String MqttPassword;
    bool IsProvisioned;
};