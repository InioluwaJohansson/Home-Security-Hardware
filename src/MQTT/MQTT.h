#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include "../Config/Config.h"

class MqttManager
{
public:

    MqttManager(ConfigurationManager& configuration);

    bool Connect();

    void Disconnect();

    bool IsConnected();

    void Loop();

    bool Publish(
        const String& topic,
        const String& payload,
        bool retained = false);

    void SubscribeTopics();

private:

    ConfigurationManager& _configuration;

    WiFiClient _wifiClient;

    PubSubClient _mqttClient;

    unsigned long _lastReconnectAttempt = 0;

    static void Callback(
        char* topic,
        byte* payload,
        unsigned int length);

    bool Reconnect();
};