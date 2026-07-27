#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include "../Config/Config.h"

class WiFiManager
{
public:

    WiFiManager(ConfigurationManager& configuration);

    bool Connect();

    void Disconnect();

    bool IsConnected() const;

    void Loop();

    String GetIPAddress() const;

private:

    ConfigurationManager& _configuration;

    unsigned long _lastReconnectAttempt = 0;

    bool Reconnect();
};