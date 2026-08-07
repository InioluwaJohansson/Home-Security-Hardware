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
    int ApplianceId;
    bool IsActive;
    int ApplianceType;
};

struct LightDeviceState
{
    int LightId;
    int BrightnessLevel;
    bool IsActive;
};

struct CameraDeviceState
{
    int CameraId;
    bool IsActive;
};

struct WindowDeviceState
{
    int WindowId;
    bool IsLocked;
    bool IsOpen;
};

struct DoorDeviceState
{
    int DoorId;
    bool IsLocked;
    bool IsOpen;
    DoorType Type;
};

struct ExternalDeviceState
{
    int ExternalId;
    bool IsActive;
    bool IsTriggered;
};

struct GenericDeviceUpdateState
{
    FacilityType Type;
    int Id;
    int BrightnessLevel;
    bool IsLocked;
    bool IsOpen;
    bool IsTriggered;
    bool IsActive;
    bool PowerActive;
    DoorType DoorCategory;
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

    unsigned long Timestamp;
    String Token;
};