#include "DeviceModels.h"
DeviceManager::DeviceManager(

    LightManager& lights,

    ApplianceManager& appliances,

    DoorManager& doors,

    WindowManager& windows,

    CameraManager& cameras,

    ExternalInputManager& externals)

:

_lights(lights),

_appliances(appliances),

_doors(doors),

_windows(windows),

_cameras(cameras),

_externals(externals)

{
}
void DeviceManager::ApplyState(const GenericDeviceState& state)
{
    for(auto& light : state.LightDeviceStates)
    {
        _lights.Apply(light);
    }

    for(auto& appliance : state.ApplianceDeviceStates)
    {
        _appliances.Apply(appliance);
    }

    for(auto& door : state.DoorDeviceStates)
    {
        _doors.Apply(door);
    }

    for(auto& window : state.WindowDeviceStates)
    {
        _windows.Apply(window);
    }

    for(auto& camera : state.CameraDeviceStates)
    {
        _cameras.Apply(camera);
    }

    for(auto& external : state.ExternalDeviceStates)
    {
        _externals.Apply(external);
    }
}
void DeviceManager::ApplyUpdate(
    const GenericDeviceUpdateState& update)
{
    switch(update.FacilityType)
    {
        case FacilityType::Light:

            _lights.Update(update);

            break;

        case FacilityType::Appliance:

            _appliances.Update(update);

            break;

        case FacilityType::Door:

            _doors.Update(update);

            break;

        case FacilityType::Window:

            _windows.Update(update);

            break;

        case FacilityType::Camera:

            _cameras.Update(update);

            break;

        case FacilityType::External:

            _externals.Update(update);

            break;
    }
}