#pragma once

#include <Arduino.h>
#include <unordered_map>

#include "DeviceModels.h"

class StateCache
{
public:

    void Initialize();

    //---------------- Lights ----------------

    void UpdateLight(const LightDeviceState& state);

    LightDeviceState GetLight(int lightId);

    bool HasLight(int lightId);

    //---------------- Appliances ----------------

    void UpdateAppliance(const ApplianceDeviceState& state);

    ApplianceDeviceState GetAppliance(int applianceId);

    bool HasAppliance(int applianceId);

    //---------------- Doors ----------------

    void UpdateDoor(const DoorDeviceState& state);

    DoorDeviceState GetDoor(int doorId);

    bool HasDoor(int doorId);

    //---------------- Windows ----------------

    void UpdateWindow(const WindowDeviceState& state);

    WindowDeviceState GetWindow(int windowId);

    bool HasWindow(int windowId);

    //---------------- Cameras ----------------

    void UpdateCamera(const CameraDeviceState& state);

    CameraDeviceState GetCamera(int cameraId);

    bool HasCamera(int cameraId);

    //---------------- Externals ----------------

    void UpdateExternal(const ExternalDeviceState& state);

    ExternalDeviceState GetExternal(int externalId);

    bool HasExternal(int externalId);

private:

    std::unordered_map<int, LightDeviceState> _lights;

    std::unordered_map<int, ApplianceDeviceState> _appliances;

    std::unordered_map<int, DoorDeviceState> _doors;

    std::unordered_map<int, WindowDeviceState> _windows;

    std::unordered_map<int, CameraDeviceState> _cameras;

    std::unordered_map<int, ExternalDeviceState> _externals;
};