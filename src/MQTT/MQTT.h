#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "../Config/Config.h"

class MqttManager
{
public:

    explicit MqttManager(ConfigurationManager& configuration);

    bool Connect();

    bool Reconnect();

    void Disconnect();

    bool IsConnected();

    void Loop();

    bool Publish(
        const String& topic,
        JsonDocument& document,
        bool retained = false);

    void SubscribeTopics();

    void PublishNFIDVerify(const NFIDVerifyDto& dto);

private:

    ConfigurationManager& _configuration;

    WiFiClient _wifiClient;

    PubSubClient _mqttClient;

    unsigned long _lastReconnectAttempt = 0;

    static void Callback(
        char* topic,
        byte* payload,
        unsigned int length);
};