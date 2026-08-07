#include "Auth.h"

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

AuthenticationManager::AuthenticationManager(
    ConfigurationManager& configuration)
    : _configuration(configuration)
{
}

bool AuthenticationManager::Authenticate()
{
    auto& config = _configuration.GetConfiguration();

    if (!WiFi.isConnected())
    {
        Serial.println(
            "[AUTH] WiFi is not connected.");

        return false;
    }

    if (config.BackendUrl.isEmpty())
    {
        Serial.println(
            "[AUTH] Backend URL is empty.");

        return false;
    }

    HTTPClient http;

    String url =
        config.BackendUrl +
        "/api/MicroController/Auth";

    Serial.print("[AUTH] Authenticating: ");
    Serial.println(url);

    if (!http.begin(url))
    {
        Serial.println(
            "[AUTH] Failed to initialize HTTP client.");

        return false;
    }

    http.addHeader(
        "Content-Type",
        "application/json");

    JsonDocument request;

    request["hardwareId"] =
        config.HardwareId;

    request["authKey"] =
        config.AuthKey;

    String body;

    serializeJson(
        request,
        body);

    int statusCode =
        http.POST(body);

    if (statusCode <= 0)
    {
        Serial.print(
            "[AUTH] HTTP error: ");

        Serial.println(
            http.errorToString(statusCode));

        http.end();

        return false;
    }

    Serial.print(
        "[AUTH] HTTP status: ");

    Serial.println(statusCode);

    if (statusCode != HTTP_CODE_OK)
    {
        String response =
            http.getString();

        Serial.println(
            "[AUTH] Authentication failed.");

        Serial.println(response);

        http.end();

        return false;
    }

    String response =
        http.getString();

    http.end();

    JsonDocument json;

    DeserializationError error =
        deserializeJson(
            json,
            response);

    if (error)
    {
        Serial.print(
            "[AUTH] Invalid JSON: ");

        Serial.println(
            error.c_str());

        return false;
    }

    bool status =
        json["status"] | false;

    if (!status)
    {
        Serial.println(
            "[AUTH] Backend rejected authentication.");

        return false;
    }

    String token =
        json["data"]["token"] | "";

    if (token.isEmpty())
    {
        Serial.println(
            "[AUTH] No JWT returned.");

        return false;
    }

    _configuration.SetJwt(token);

    Serial.println(
        "[AUTH] Authentication successful.");

    return true;
}