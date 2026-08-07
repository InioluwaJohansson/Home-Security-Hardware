#pragma once

#include <Arduino.h>

struct Config
{
    String HardwareId;

    String AuthKey;

    String WifiSSID;

    String WifiPassword;

    String BackendUrl;

    String MqttHost;

    uint16_t MqttPort = 1883;

    String MqttUsername;

    String MqttPassword;

    uint16_t HeartbeatInterval = 30;

    bool IsProvisioned = false;
};