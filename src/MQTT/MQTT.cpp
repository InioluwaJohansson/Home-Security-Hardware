#include "MQTT.h"
#include <Arduino.h>
#include "MessageRouter.h"

MessageRouter* MqttManager::_router = nullptr;

MqttManager::MqttManager(
    ConfigurationManager& configuration)
    :
    _configuration(configuration),
    _mqttClient(_wifiClient)
{
}

void MqttManager::SetMessageRouter(
    MessageRouter& router)
{
    _router = &router;
}

bool MqttManager::Connect()
{
    auto& config =
        _configuration.GetConfiguration();

    _mqttClient.setServer(
        config.MqttHost.c_str(),
        config.MqttPort);

    _mqttClient.setCallback(Callback);

    return Reconnect();
}

bool MqttManager::Reconnect()
{
    auto& config =
        _configuration.GetConfiguration();

    if(_mqttClient.connected())
        return true;

    bool connected =
        _mqttClient.connect(
            config.HardwareId.c_str(),
            config.MqttUsername.c_str(),
            config.MqttPassword.c_str());

    if(!connected)
    {
        return false;
    }

    SubscribeTopics();

    JsonDocument doc;

    doc["online"] = true;

    Publish(
        "home/" + config.HardwareId + "/status",
        doc,
        true);

    _lastReconnectAttempt = millis();

    return true;
}

void MqttManager::Disconnect()
{
    if(_mqttClient.connected())
    {
        _mqttClient.disconnect();
    }
}

bool MqttManager::IsConnected()
{
    return _mqttClient.connected();
}

void MqttManager::Loop()
{
    if(!_mqttClient.connected())
    {
        unsigned long now = millis();

        if(now - _lastReconnectAttempt >= 5000)
        {
            _lastReconnectAttempt = now;

            Reconnect();
        }

        return;
    }

    _mqttClient.loop();
}

void MqttManager::SubscribeTopics()
{
    auto& config =
        _configuration.GetConfiguration();

    String base =
        "home/" + config.HardwareId;

    _mqttClient.subscribe(
        (base + "/command").c_str());

    _mqttClient.subscribe(
        (base + "/sync").c_str());

    _mqttClient.subscribe(
        (base + "/fingerprint").c_str());

    _mqttClient.subscribe(
        (base + "/fingerprint/verify/response").c_str());

    _mqttClient.subscribe(
        (base + "/NFID").c_str());

    _mqttClient.subscribe(
        (base + "/NFID/Data").c_str());

    _mqttClient.subscribe(
        (base + "/Hardware/Auth/Response").c_str());
}

bool MqttManager::Publish(
    const String& topic,
    JsonDocument& document,
    bool retained)
{
    if(!_mqttClient.connected())
    {
        return false;
    }

    String json;

    serializeJson(
        document,
        json);

    return _mqttClient.publish(
        topic.c_str(),
        json.c_str(),
        retained);
}

void MqttManager::Callback(
    char* topic,
    byte* payload,
    unsigned int length)
{
    String message;

    message.reserve(length);

    for(unsigned int i = 0;
        i < length;
        i++)
    {
        message +=
            static_cast<char>(payload[i]);
    }

    Serial.print("[MQTT] ");

    Serial.println(topic);

    Serial.println(message);

    if(_router == nullptr)
    {
        Serial.println(
            "[MQTT] MessageRouter not connected");

        return;
    }

    _router->Route(
        String(topic),
        message);
}

void MqttManager::PublishHeartbeat(
    const HardwareHeartBeatDto& dto)
{
    JsonDocument doc;

    doc["Token"] = dto.Token;

    doc["HardwareId"] =
        dto.HardwareId;

    doc["PowerActive"] =
        dto.PowerActive;

    Publish(
        "home/" +
        dto.HardwareId +
        "/heartbeat",
        doc);
}

void MqttManager::PublishNFIDVerify(
    const NFIDVerifyDto& dto)
{
    JsonDocument doc;

    doc["Token"] =
        dto.Token;

    doc["HardwareId"] =
        dto.HardwareId;

    doc["NFIDCode"] =
        dto.NFIDCode;

    Publish(
        "home/" +
        dto.HardwareId +
        "/NFID/verify",
        doc);
}