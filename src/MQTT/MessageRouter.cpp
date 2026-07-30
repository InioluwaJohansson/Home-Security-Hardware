#include "MessageRouter.h"

#include "../Config/Config.h"

MessageRouter::MessageRouter(
    ConfigurationManager& configuration,
    DeviceManager& deviceManager,
    Access& access)
    :
    _configuration(configuration),
    _deviceManager(deviceManager),
    _access(access)
{
}
void MessageRouter::Route(
    const String& topic,
    const String& payload)
{
    StaticJsonDocument<4096> doc;

    auto error = deserializeJson(doc, payload);

    if(error)
    {
        Serial.println("Invalid JSON");
        return;
    }

    if(topic.endsWith("/hardware/auth/response"))
    {
        HandleAuthentication(doc);
        return;
    }

    if(topic.endsWith("/sync"))
    {
        HandleSync(doc);
        return;
    }

    if(topic.endsWith("/command"))
    {
        HandleCommand(doc);
        return;
    }

    if(topic.endsWith("/fingerprint/verify/response"))
    {
        HandleFingerprint(doc);
        return;
    }

    if(topic.endsWith("/NFID/Data"))
    {
        HandleRFID(doc);
        return;
    }
}
void MessageRouter::HandleAuthentication(
    const JsonDocument& doc)
{
    bool status = doc["status"];

    if(!status)
    {
        Serial.println("Authentication Failed");
        return;
    }

    String token = doc["token"];

    _configuration.SetJwt(token);

    Serial.println("JWT Saved");

    HandleSync(doc);
}
void MessageRouter::HandleSync(
    const JsonDocument& doc)
{
    _deviceManager.ApplyState(doc);
}
void MessageRouter::HandleCommand(
    const JsonDocument& doc)
{
    _deviceManager.ApplyUpdate(doc);
}
void MessageRouter::HandleFingerprint(
    const JsonDocument& doc)
{
    bool status = doc["status"] | false;

    if(status)
    {
        _access.Grant();
    }
    else
    {
        _access.Deny();
    }
}
void MessageRouter::HandleRFID(
    const JsonDocument& doc)
{
    bool status = doc["status"] | false;

    if(status)
    {
        _access.Grant();
    }
    else
    {
        _access.Deny();
    }
}
