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
    JsonDocument doc;

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

    // ArduinoJson 7 does not automatically deserialize std::vector-based
    // application models. Map each array explicitly.
    auto first = [](JsonVariantConst root,
                    const char* pascal,
                    const char* camel) -> JsonVariantConst
    {
        JsonVariantConst value = root[pascal];
        if (!value.isNull())
            return value;

        return root[camel];
    };

    JsonArrayConst appliances =
        first(doc, "ApplianceDeviceStates", "applianceDeviceStates");
    for (JsonObjectConst item : appliances)
    {
        ApplianceDeviceState value;
        value.ApplianceId =
            first(item, "ApplianceId", "applianceId") | -1;
        value.IsActive =
            first(item, "IsActive", "isActive") | false;
        value.ApplianceType =
            first(item, "ApplianceType", "applianceType") | 0;

        state.ApplianceDeviceStates.push_back(value);
    }

    JsonArrayConst lights =
        first(doc, "LightDeviceStates", "lightDeviceStates");
    for (JsonObjectConst item : lights)
    {
        LightDeviceState value;
        value.LightId =
            first(item, "LightId", "lightId") | -1;
        value.BrightnessLevel =
            first(item, "BrightnessLevel", "brightnessLevel") | 0;
        value.IsActive =
            first(item, "IsActive", "isActive") | false;

        state.LightDeviceStates.push_back(value);
    }

    JsonArrayConst cameras =
        first(doc, "CameraDeviceStates", "cameraDeviceStates");
    for (JsonObjectConst item : cameras)
    {
        CameraDeviceState value;
        value.CameraId =
            first(item, "CameraId", "cameraId") | -1;
        value.IsActive =
            first(item, "IsActive", "isActive") | false;

        state.CameraDeviceStates.push_back(value);
    }

    JsonArrayConst windows =
        first(doc, "WindowDeviceStates", "windowDeviceStates");
    for (JsonObjectConst item : windows)
    {
        WindowDeviceState value;
        value.WindowId =
            first(item, "WindowId", "windowId") | -1;
        value.IsLocked =
            first(item, "IsLocked", "isLocked") | false;
        value.IsOpen =
            first(item, "IsOpen", "isOpen") | false;

        state.WindowDeviceStates.push_back(value);
    }

    JsonArrayConst doors =
        first(doc, "DoorDeviceStates", "doorDeviceStates");
    for (JsonObjectConst item : doors)
    {
        DoorDeviceState value;
        value.DoorId =
            first(item, "DoorId", "doorId") | -1;
        value.IsLocked =
            first(item, "IsLocked", "isLocked") | false;
        value.IsOpen =
            first(item, "IsOpen", "isOpen") | false;

        const int type =
            first(item, "DoorType", "doorType") |
            static_cast<int>(DoorType::Interior);

        if (type >= static_cast<int>(DoorType::Interior) &&
            type <= static_cast<int>(DoorType::Gate))
        {
            value.Type = static_cast<DoorType>(type);
        }
        else
        {
            value.Type = DoorType::Interior;
        }

        state.DoorDeviceStates.push_back(value);
    }

    JsonArrayConst externals =
        first(doc, "ExternalDeviceStates", "externalDeviceStates");
    for (JsonObjectConst item : externals)
    {
        ExternalDeviceState value;
        value.ExternalId =
            first(item, "ExternalId", "externalId") | -1;
        value.IsActive =
            first(item, "IsActive", "isActive") | false;
        value.IsTriggered =
            first(item, "IsTriggered", "isTriggered") | false;

        state.ExternalDeviceStates.push_back(value);
    }

    state.Timestamp =
        first(doc, "Timestamp", "timestamp") | 0UL;
    state.Token =
        first(doc, "Token", "token") | "";

    // A sync message is still valid when it does not contain a JWT.
    // Only update authentication state when a token was actually supplied.
    if (!state.Token.isEmpty())
    {
        _configuration.SetJwt(state.Token);
        _configuration.Save();
        Serial.println("JWT Saved");
    }

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