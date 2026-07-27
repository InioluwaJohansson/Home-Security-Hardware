#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>

#include "../Config/Config.h"

class Provisioning
{
public:

    Provisioning(
        ConfigurationManager& configuration);

    bool Begin();

    void Loop();

    bool IsRunning() const;

private:

    ConfigurationManager& _configuration;

    WebServer _server{80};

    DNSServer _dns;

    bool _running = false;

    void StartAccessPoint();

    void ConfigureRoutes();

    void HandleRoot();

    void HandleStyle();

    void HandleScript();

    void HandleScan();

    void HandleProvision();

    void HandleNotFound();

    bool Provision(
        const JsonDocument& request,
        String& message);

    bool DownloadConfiguration(
        const String& backendUrl,
        const String& hardwareId,
        const String& authKey,
        Config& config,
        String& message);

    void SendResponse(
        bool status,
        const String& message);
};