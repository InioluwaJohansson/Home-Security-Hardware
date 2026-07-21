#include <Arduino.h>
#include "MQTT.h"
#include <ArduinoJson.h>

MqttManager::MqttManager(ConfigurationManager& configuration)
    : _configuration(configuration),
      _mqttClient(_wifiClient)
{
}

bool MqttManager::Connect()
{
    auto& config = _configuration.GetConfiguration();

    _mqttClient.setServer(
        config.MqttHost.c_str(),
        config.MqttPort);

    _mqttClient.setCallback(Callback);

    return Reconnect();
}
bool MqttManager::Reconnect()
{
    auto& config = _configuration.GetConfiguration();

    if (_mqttClient.connected())return true;

    bool connected = _mqttClient.connect(config.HardwareId.c_str());

    if (connected)
    {
        SubscribeTopics();
        Publish(
            "home/" + config.HardwareId + "/status",
            "{\"online\":true}",
            true);
    }
    return connected;
}
void MqttManager::Loop()
{
    if (!_mqttClient.connected())
    {
        unsigned long now = millis();
        if (now - _lastReconnectAttempt > 5000)
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
    auto& config = _configuration.GetConfiguration();

    String base = "home/" + config.HardwareId;

    _mqttClient.subscribe((base + "/command").c_str());

    _mqttClient.subscribe((base + "/sync").c_str());

    _mqttClient.subscribe((base + "/fingerprint/verify/response").c_str());

    _mqttClient.subscribe((base + "/NFID/Data").c_str());

    _mqttClient.subscribe((base + "/hardware/auth/response").c_str());
}
bool MqttManager::Publish(const String& topic, JsonDocument& document, bool retained)
{
    if (!_mqttClient.connected()) return false;
    document["token"] = _configuration.GetJwt();
    String json;
    serializeJson(document, json);
    return _mqttClient.publish(topic.c_str(), json.c_str(), retained);
}
void MqttManager::Callback(char* topic, byte* payload, unsigned int length)
{
    String message;
    for(unsigned int i=0;i<length;i++)
    {
        message += (char)payload[i];
    }

    Serial.println(topic);
    Serial.println(message);

    // Next lesson:
    // MessageRouter.Route(topic,message);
}