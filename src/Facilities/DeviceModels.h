#pragma once

#include <Arduino.h>
#include <vector>

enum class FacilityType
{
    Appliance = 1,
    Light = 2,
    Camera = 3,
    Window = 4,
    Door = 5,
    External = 6
};

enum class DoorType
{
    Interior = 1,
    Exterior = 2,
    Gate = 3
};

struct ApplianceDeviceState
{
    int ApplianceId = -1;
    bool IsActive = false;
    int ApplianceType = 0;
};

struct LightDeviceState
{
    int LightId = -1;
    int BrightnessLevel = 0;
    bool IsActive = false;
};

struct CameraDeviceState
{
    int CameraId = -1;
    bool IsActive = false;
};

struct WindowDeviceState
{
    int WindowId = -1;
    bool IsLocked = false;
    bool IsOpen = false;
};

struct DoorDeviceState
{
    int DoorId = -1;
    bool IsLocked = false;
    bool IsOpen = false;
    DoorType Type = DoorType::Interior;
};

struct ExternalDeviceState
{
    int ExternalId = -1;

    // IsActive means the external sensor/input is enabled.
    bool IsActive = false;

    // IsTriggered is the actual physical sensor state.
    bool IsTriggered = false;
};

struct GenericDeviceUpdateState
{
    FacilityType Type = FacilityType::External;
    int Id = -1;
    int BrightnessLevel = 0;
    bool IsLocked = false;
    bool IsOpen = false;
    bool IsTriggered = false;
    bool IsActive = false;
    bool PowerActive = false;
    String Token;
};

struct GenericDeviceState
{
    std::vector<ApplianceDeviceState> ApplianceDeviceStates;
    std::vector<LightDeviceState> LightDeviceStates;
    std::vector<CameraDeviceState> CameraDeviceStates;
    std::vector<WindowDeviceState> WindowDeviceStates;
    std::vector<DoorDeviceState> DoorDeviceStates;
    std::vector<ExternalDeviceState> ExternalDeviceStates;

    unsigned long Timestamp = 0;
    String Token;
};
