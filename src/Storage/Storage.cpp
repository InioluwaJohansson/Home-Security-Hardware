#include "Storage.h"
#include <Preferences.h>
Preferences preferences;

bool StorageManager::Begin()
{
    return preferences.begin(Namespace, false);
}

bool StorageManager::SaveConfiguration(const DeviceConfiguration& config)
{
    preferences.putString("hardwareId", config.HardwareId);
    preferences.putString("authKey", config.AuthKey);
    preferences.putString("wifiSsid", config.WifiSSID);
    preferences.putString("wifiPass", config.WifiPassword);
    preferences.putString("mqttHost", config.MqttHost);
    preferences.putUShort("mqttPort", config.MqttPort);
    preferences.putString("mqttHost", config.MqttUsername);
    preferences.putUShort("mqttPort", config.MqttPassword);
    preferences.putString("deviceName", config.DeviceName);
    preferences.putBool("provisioned", config.IsProvisioned);
    return true;
}

DeviceConfiguration StorageManager::LoadConfiguration()
{
    DeviceConfiguration config;
    config.HardwareId = preferences.getString("hardwareId", "");
    config.AuthKey = preferences.getString("authKey", "");
    config.WifiSSID = preferences.getString("wifiSsid", "");
    config.WifiPassword = preferences.getString("wifiPass", "");
    config.MqttHost = preferences.getString("mqttHost", "");
    config.MqttPort = preferences.getUShort("mqttPort", 1883);
    config.MqttHost = preferences.getString("mqttUsername", "");
    config.MqttPort = preferences.getUShort("mqttPassword", 1883);
    return config;
}

void StorageManager::Clear()
{
    preferences.clear();
}