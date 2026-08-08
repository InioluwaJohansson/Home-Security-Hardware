#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Provisioning.h"

Provisioning::Provisioning(
    ConfigurationManager& configuration)
    :
    _configuration(configuration)
{
}
bool Provisioning::Begin()
{
    auto& config = _configuration.GetConfiguration();

    if (config.IsProvisioned &&
        !config.WifiSSID.isEmpty())
    {
        WiFi.mode(WIFI_STA);

        WiFi.begin(
            config.WifiSSID.c_str(),
            config.WifiPassword.c_str());

        const unsigned long start = millis();

        while (WiFi.status() != WL_CONNECTED &&
               millis() - start < 15000)
        {
            delay(250);
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            _running = false;
            return true;
        }

        WiFi.disconnect(true);
    }

    StartAccessPoint();
    ConfigureRoutes();
    _server.begin();

    _running = true;
    return true;
}
void Provisioning::Loop()
{
    if(!_running)
        return;

    _dns.processNextRequest();

    _server.handleClient();
}
bool Provisioning::IsRunning() const
{
    return _running;
}
void Provisioning::ConfigureRoutes()
{
    _server.on(
        "/",
        HTTP_GET,
        std::bind(
            &Provisioning::HandleRoot,
            this));

    _server.on(
        "/style.css",
        HTTP_GET,
        std::bind(
            &Provisioning::HandleStyle,
            this));

    _server.on(
        "/app.js",
        HTTP_GET,
        std::bind(
            &Provisioning::HandleScript,
            this));

    _server.on(
        "/api/scan",
        HTTP_GET,
        std::bind(
            &Provisioning::HandleScan,
            this));

    _server.on(
        "/api/provision",
        HTTP_POST,
        std::bind(
            &Provisioning::HandleProvision,
            this));

    _server.onNotFound(
        std::bind(
            &Provisioning::HandleNotFound,
            this));
}
void Provisioning::HandleProvision()
{
    StaticJsonDocument<1024> request;

    auto error =
        deserializeJson(
            request,
            _server.arg("plain"));

    if(error)
    {
        SendResponse(
            false,
            "Invalid request");

        return;
    }

    String message;

    bool success =
        Provision(
            request,
            message);

    SendResponse(
        success,
        message);
}
bool Provisioning::Provision(
    const JsonDocument& request,
    String& message)
{
    auto& config =
        _configuration.GetConfiguration();

    config.HardwareId =
        request["hardwareId"].as<String>();

    config.AuthKey =
        request["authKey"].as<String>();

    config.WifiSSID =
        request["wifiSsid"].as<String>();

    config.WifiPassword =
        request["wifiPassword"].as<String>();

    config.BackendUrl =
        request["backendUrl"].as<String>();

    WiFi.disconnect(true);

    WiFi.begin(
        config.WifiSSID.c_str(),
        config.WifiPassword.c_str());

    unsigned long start =
        millis();

    while(WiFi.status()!=WL_CONNECTED)
    {
        if(millis()-start>15000)
        {
            message =
                "Unable to connect to WiFi";

            return false;
        }

        delay(500);
    }

    if(!DownloadConfiguration(
        config.BackendUrl,
        config.HardwareId,
        config.AuthKey,
        config,
        message))
    {
        return false;
    }

    config.IsProvisioned = true;

    _configuration.Save();

    _running = false;

    message =
        "Provision successful";

    return true;
}
bool Provisioning::DownloadConfiguration(
    const String& backendUrl,
    const String& hardwareId,
    const String& authKey,
    Config& config,
    String& message)
{
    HTTPClient http;

    String url = backendUrl;

    if (!url.endsWith("/"))
    {
        url += "/";
    }

    url += "api/MicroController/Provision";

    if (!http.begin(url))
    {
        message = "Unable to connect to backend.";
        return false;
    }

    http.addHeader(
        "Content-Type",
        "application/json");

    StaticJsonDocument<256> request;

    request["hardwareId"] = hardwareId;
    request["authKey"] = authKey;

    String body;

    serializeJson(request, body);

    int statusCode = http.POST(body);

    if (statusCode <= 0)
    {
        message = http.errorToString(statusCode);
        http.end();
        return false;
    }

    if (statusCode != HTTP_CODE_OK)
    {
        message = "Provisioning failed.";
        http.end();
        return false;
    }

    StaticJsonDocument<2048> response;

    auto error = deserializeJson(
        response,
        http.getString());

    http.end();

    if (error)
    {
        message = "Invalid server response.";
        return false;
    }

    bool status = response["status"] | false;

    if (!status)
    {
        message = response["message"] | "Provisioning failed.";
        return false;
    }

    JsonObject data = response["data"];

    config.MqttHost =
        data["mqttHost"] | "";

    config.MqttPort =
        data["mqttPort"] | 1883;

    config.MqttUsername =
        data["mqttUsername"] | "";

    config.MqttPassword =
        data["mqttPassword"] | "";

    message =
        response["message"] | "Provisioning successful.";

    return true;
}
void Provisioning::SendResponse(
    bool status,
    const String& message)
{
    StaticJsonDocument<256> json;

    json["status"] = status;

    json["message"] = message;

    String response;

    serializeJson(
        json,
        response);

    _server.send(
        200,
        "application/json",
        response);
}