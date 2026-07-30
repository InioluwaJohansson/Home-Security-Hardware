#pragma once

#include <Arduino.h>

class ConfigurationManager;
class MqttManager;

class Heartbeat
{
public:

    Heartbeat(
        ConfigurationManager& configuration, MqttManager& mqtt);

    void Update();

private:

    ConfigurationManager& _configuration;

    MqttManager& _mqtt;

    unsigned long _lastHeartbeat;

    static constexpr unsigned long Interval = 120000;
};
struct HardwareHeartBeatDto
{
    String Token;

    String HardwareId;

    bool PowerActive;
};