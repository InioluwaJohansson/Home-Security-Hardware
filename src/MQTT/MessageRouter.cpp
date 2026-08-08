#include "MessageRouter.h"

#include "../Config/Config.h"
#include "../Facilities/Devices.h"
#include "../Access/Access.h"
#include "../Fingerprint/Fingerprint.h"
#include "../NFID/NFID.h"

MessageRouter::MessageRouter(
    ConfigurationManager& configuration,
    DeviceManager& deviceManager,
    Access& access,
    Fingerprint& fingerprint,
    NFID& nfid)
    :
    _configuration(configuration),
    _deviceManager(deviceManager),
    _access(access),
    _fingerprint(fingerprint),
    _nfid(nfid)
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

    if(topic.endsWith("/fingerprint"))
    {
        HandleFingerprintRequest(doc);
        return;
    }

    if(topic.endsWith("/fingerprint/verify/response"))
    {
        HandleFingerprint(doc);
        return;
    }

    if(topic.endsWith("/NFID"))
    {
        HandleNFIDWrite(doc);
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
    bool status = doc["status"] | false;

    if(!status)
    {
        Serial.println("Authentication Failed");
        return;
    }

    String token = doc["token"] | "";

    if(token.isEmpty())
    {
        Serial.println("Authentication response contains no token");
        return;
    }

    _configuration.SetJwt(token);

    _configuration.Save();

    Serial.println("JWT Saved");
}

void MessageRouter::HandleSync(
    const JsonDocument& doc)
{
    GenericDeviceState state;

    deserializeJson(
        state,
        doc.as<GenericDeviceState>());
        // GenericDeviceState state = doc.as<GenericDeviceState>();
    String token = doc["token"] | "";

    if(token.isEmpty())
    {
        Serial.println("Authentication response contains no token");
        return;
    }

    _configuration.SetJwt(token);

    _configuration.Save();

    Serial.println("JWT Saved");

    _deviceManager.ApplyState(state);
}

void MessageRouter::HandleCommand(
    const JsonDocument& doc)
{
    GenericDeviceUpdateState update;

    update.Type =
        static_cast<FacilityType>(
            doc["FacilityType"] | 0);

    update.Id =
        doc["Id"] | 0;

    update.BrightnessLevel =
        doc["BrightnessLevel"] | 0;

    update.IsLocked =
        doc["IsLocked"] | false;

    update.IsOpen =
        doc["IsOpen"] | false;

    update.IsTriggered =
        doc["IsTriggered"] | false;

    update.IsActive =
        doc["IsActive"] | false;

    update.PowerActive =
        doc["PowerActive"] | false;

    update.Token =
        doc["Token"] | "";

    _deviceManager.ApplyUpdate(update);
}

void MessageRouter::HandleHeartbeat(
    const JsonDocument& doc)
{
    // Heartbeat responses can be handled here
    // if the backend sends one.
}

void MessageRouter::HandleFingerprintRequest(
    const JsonDocument& doc)
{
    int personId = doc["personId"] | -1;

    if(personId < 0)
    {
        Serial.println("Invalid fingerprint request");
        return;
    }

    _fingerprint.RequestCapture(personId);
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

void MessageRouter::HandleNFIDWrite(
    const JsonDocument& doc)
{
    String nfidCode = doc["NFIDCode"] | "";

    if(nfidCode.isEmpty())
    {
        Serial.println("Invalid NFID code");
        return;
    }

    _nfid.QueueWrite(nfidCode);
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