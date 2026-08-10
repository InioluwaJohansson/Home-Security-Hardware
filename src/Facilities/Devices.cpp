#include "DeviceModels.h"
#include "Devices.h"

#include "Light.h"
#include "Appliance.h"
#include "Door.h"
#include "Window.h"
#include "Camera.h"
#include "External.h"
DeviceManager::DeviceManager(
    LightManager& lights,
    ApplianceManager& appliances,
    DoorManager& doors,
    WindowManager& windows,
    CameraManager& cameras,
    ExternalManager& externals,
    StateCache& stateCache)
    :
    _lights(lights),
    _appliances(appliances),
    _doors(doors),
    _windows(windows),
    _cameras(cameras),
    _externals(externals),
    _stateCache(stateCache)
{
    auto callback =
        [this](
            const GenericDeviceUpdateState& update)
        {
            OnStateChanged(update);
        };

    _lights.StateChanged = callback;
    _appliances.StateChanged = callback;
    _doors.StateChanged = callback;
    _windows.StateChanged = callback;
    _cameras.StateChanged = callback;
    _externals.StateChanged = callback;
}
void DeviceManager::ApplyState(const GenericDeviceState& state)
{
    // These changes originated from the backend. Do not echo them back
    // to the backend as hardware-originated state changes.
    _suppressStateChanged = true;
    for(auto& light : state.LightDeviceStates)
    {
        _stateCache.UpdateLight(light);
        _lights.Apply(light);
    }

    for(auto& appliance : state.ApplianceDeviceStates)
    {
        _stateCache.UpdateAppliance(appliance);
        _appliances.Apply(appliance);
    }

    for(auto& door : state.DoorDeviceStates)
    {
        _stateCache.UpdateDoor(door);
        _doors.Apply(door);
    }

    for(auto& window : state.WindowDeviceStates)
    {
        _stateCache.UpdateWindow(window);
        _windows.Apply(window);
    }

    for(auto& camera : state.CameraDeviceStates)
    {
        _stateCache.UpdateCamera(camera);
        _cameras.Apply(camera);
    }

    for(auto& external : state.ExternalDeviceStates)
    {
        _stateCache.UpdateExternal(external);
        _externals.Apply(external);
    }

    _suppressStateChanged = false;
}

void DeviceManager::ApplyUpdate(
    const GenericDeviceUpdateState& update)
{
    // A command came from the backend. The resulting manager callback must
    // not be published back as a new hardware event.
    _suppressStateChanged = true;
    switch(update.Type)
    {
        case FacilityType::Light:
        {
            LightDeviceState state = _stateCache.GetLight(update.Id);
            state.LightId = update.Id;
            state.IsActive = update.IsActive;
            state.BrightnessLevel = update.BrightnessLevel;
            _stateCache.UpdateLight(state);
            _lights.Update(update);
            break;
        }

        case FacilityType::Appliance:
        {
            ApplianceDeviceState state = _stateCache.GetAppliance(update.Id);
            state.ApplianceId = update.Id;
            state.IsActive = update.IsActive;
            _stateCache.UpdateAppliance(state);
            _appliances.Update(update);
            break;
        }

        case FacilityType::Door:
        {
            DoorDeviceState state = _stateCache.GetDoor(update.Id);
            state.DoorId = update.Id;
            state.IsLocked = update.IsLocked;
            state.IsOpen = update.IsOpen;
            _stateCache.UpdateDoor(state);
            _doors.Update(update);
            break;
        }

        case FacilityType::Window:
        {
            WindowDeviceState state = _stateCache.GetWindow(update.Id);
            state.WindowId = update.Id;
            state.IsLocked = update.IsLocked;
            state.IsOpen = update.IsOpen;
            _stateCache.UpdateWindow(state);
            _windows.Update(update);
            break;
        }

        case FacilityType::Camera:
        {
            CameraDeviceState state = _stateCache.GetCamera(update.Id);
            state.CameraId = update.Id;
            state.IsActive = update.IsActive;
            _stateCache.UpdateCamera(state);
            _cameras.Update(update);
            break;
        }

        case FacilityType::External:
        {
            ExternalDeviceState state = _stateCache.GetExternal(update.Id);
            state.ExternalId = update.Id;
            state.IsActive = update.IsActive;
            state.IsTriggered = update.IsTriggered;
            _stateCache.UpdateExternal(state);
            _externals.Update(update);
            break;
        }
    }

    _suppressStateChanged = false;
}

void DeviceManager::NotifyPhysicalStateChanged(
    const GenericDeviceUpdateState& update)
{
    OnStateChanged(update);
}

void DeviceManager::OnStateChanged(
    const GenericDeviceUpdateState& update)
{
    if(_suppressStateChanged)
        return;

    if(DeviceStateChanged)
    {
        DeviceStateChanged(update);
    }
}