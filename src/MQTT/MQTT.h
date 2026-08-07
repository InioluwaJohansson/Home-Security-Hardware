#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include "../Config/Config.h"
#include "../Managers/Heartbeat.h"
#include "../NFID/NFIDModels.h"

class MessageRouter;

class MqttManager
{
public:

    explicit MqttManager(
        ConfigurationManager& configuration);

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

    void SetMessageRouter(
        MessageRouter& router);

    void PublishHeartbeat(
        const HardwareHeartBeatDto& dto);

    void PublishNFIDVerify(
        const NFIDVerifyDto& dto);

private:

    ConfigurationManager& _configuration;

    WiFiClient _wifiClient;

    PubSubClient _mqttClient;

    unsigned long _lastReconnectAttempt = 0;

    static MessageRouter* _router;

    static void Callback(
        char* topic,
        byte* payload,
        unsigned int length);
};