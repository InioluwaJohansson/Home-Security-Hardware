#include "Auth.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

AuthenticationManager::AuthenticationManager(
    ConfigurationManager& configuration)
    : _configuration(configuration)
{
}

bool AuthenticationManager::Authenticate()
{
    auto& config = _configuration.GetConfiguration();

    HTTPClient http;

    String url =
        "http://" +
        config.ServerHost +
        ":" +
        String(config.ServerPort) +
        "/api/MicroController/Auth";

    http.begin(url);

    http.addHeader(
        "Content-Type",
        "application/json");

    StaticJsonDocument<256> request;

    request["hardwareId"] = config.HardwareId;

    request["authKey"] = config.AuthKey;

    String body;

    serializeJson(request, body);

    int statusCode = http.POST(body);

    if (statusCode != HTTP_CODE_OK)
    {
        http.end();
        return false;
    }

    String response = http.getString();

    http.end();

    StaticJsonDocument<2048> json;

    auto error = deserializeJson(
        json,
        response);

    if (error)
        return false;

    if (!json["status"])
        return false;

    String token = json["data"]["token"];

    _configuration.SetJwt(token);

    return true;
}