#include "WiFi.h"

WiFiManager::WiFiManager(
    ConfigurationManager& configuration)
    :
    _configuration(configuration)
{
}

bool WiFiManager::Connect()
{
    auto& config = _configuration.GetConfiguration();

    if(config.WifiSSID.isEmpty())
        return false;

    WiFi.mode(WIFI_STA);

    WiFi.begin(
        config.WifiSSID.c_str(),
        config.WifiPassword.c_str());

    unsigned long start = millis();

    while(WiFi.status() != WL_CONNECTED)
    {
        if(millis() - start > 15000)
        {
            return false;
        }

        delay(500);
    }

    return true;
}

bool WiFiManager::Reconnect()
{
    if(IsConnected())
        return true;

    return Connect();
}

void WiFiManager::Loop()
{
    if(IsConnected())
        return;

    unsigned long now = millis();

    if(now - _lastReconnectAttempt < 5000)
        return;

    _lastReconnectAttempt = now;

    Reconnect();
}

void WiFiManager::Disconnect()
{
    WiFi.disconnect(true);
}

bool WiFiManager::IsConnected() const
{
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::GetIPAddress() const
{
    return WiFi.localIP().toString();
}