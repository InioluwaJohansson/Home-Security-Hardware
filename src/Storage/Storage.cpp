#include "Storage.h"
#include <Preferences.h>
Preferences preferences;

bool StorageManager::Begin()
{
    return preferences.begin(Namespace, false);
}

bool StorageManager::SaveConfiguration(const Config& config)
{
    preferences.putString("hardwareId", config.HardwareId);
    preferences.putString("authKey", config.AuthKey);
    preferences.putString("wifiSsid", config.WifiSSID);
    preferences.putString("wifiPass", config.WifiPassword);
    preferences.putString("mqttHost", config.MqttHost);
    preferences.putUShort("mqttPort", config.MqttPort);
    preferences.putString("mqttUsername", config.MqttUsername);
    preferences.putUShort("mqttPassword", config.MqttPassword);
    preferences.putUShort("heartbeatInterval", config.HeartbeatInterval);
    preferences.putBool("isProvisioned", config.IsProvisioned);
    return true;
}

Config StorageManager::LoadConfiguration()
{
    Config config;
    config.HardwareId = preferences.getString("hardwareId", "");
    config.AuthKey = preferences.getString("authKey", "");
    config.WifiSSID = preferences.getString("wifiSsid", "");
    config.WifiPassword = preferences.getString("wifiPass", "");
    config.MqttHost = preferences.getString("mqttHost", "");
    config.MqttPort = preferences.getUShort("mqttPort", 1883);
    config.MqttUsername = preferences.getString("mqttUsername", "");
    config.MqttPassword = preferences.getUShort("mqttPassword", "");
    config.MqttPort = preferences.getUShort("heartbeatInterval", 1883);
    config.IsProvisioned = preferences.getUShort("isProvisioned", false);
    return config;
}

void StorageManager::Clear()
{
    preferences.clear();
}
bool StorageManager::IsProvisioned()
{
    return
        preferences.getString("hardwareId", "").length() > 0 &&
        preferences.getString("authKey", "").length() > 0;
}