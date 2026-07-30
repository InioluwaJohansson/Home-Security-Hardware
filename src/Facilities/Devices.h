#pragma once

#include <Arduino.h>

#include "../Models/DeviceModels.h"
#include <functional>
class LightManager;
class ApplianceManager;
class DoorManager;
class WindowManager;
class CameraManager;
class ExternalInputManager;

class DeviceManager
{
public:
    std::function<void(const GenericDeviceUpdateState&)> DeviceStateChanged;

    DeviceManager(
        LightManager& lights,
        ApplianceManager& appliances,
        DoorManager& doors,
        WindowManager& windows,
        CameraManager& cameras,
        ExternalInputManager& externals);

    void ApplyState(
        const GenericDeviceState& state);

    void ApplyUpdate(
        const GenericDeviceUpdateState& update);

private:

    LightManager& _lights;
    ApplianceManager& _appliances;
    DoorManager& _doors;
    WindowManager& _windows;
    CameraManager& _cameras;
    ExternalInputManager& _externals;

    void OnStateChanged(const GenericDeviceUpdateState& update);
};
