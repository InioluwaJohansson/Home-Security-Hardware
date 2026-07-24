#include "StateCache.h"

void StateCache::Initialize()
{
    _lights.clear();
    _appliances.clear();
    _doors.clear();
    _windows.clear();
    _cameras.clear();
    _externals.clear();
}

//---------------- Lights ----------------

void StateCache::UpdateLight(const LightDeviceState& state)
{
    _lights[state.LightId] = state;
}

LightDeviceState StateCache::GetLight(int lightId)
{
    auto it = _lights.find(lightId);

    if (it != _lights.end())
        return it->second;

    return {};
}

bool StateCache::HasLight(int lightId)
{
    return _lights.find(lightId) != _lights.end();
}

//---------------- Appliances ----------------

void StateCache::UpdateAppliance(const ApplianceDeviceState& state)
{
    _appliances[state.ApplianceId] = state;
}

ApplianceDeviceState StateCache::GetAppliance(int applianceId)
{
    auto it = _appliances.find(applianceId);

    if (it != _appliances.end())
        return it->second;

    return {};
}

bool StateCache::HasAppliance(int applianceId)
{
    return _appliances.find(applianceId) != _appliances.end();
}

//---------------- Doors ----------------

void StateCache::UpdateDoor(const DoorDeviceState& state)
{
    _doors[state.DoorId] = state;
}

DoorDeviceState StateCache::GetDoor(int doorId)
{
    auto it = _doors.find(doorId);

    if (it != _doors.end())
        return it->second;

    return {};
}

bool StateCache::HasDoor(int doorId)
{
    return _doors.find(doorId) != _doors.end();
}

//---------------- Windows ----------------

void StateCache::UpdateWindow(const WindowDeviceState& state)
{
    _windows[state.WindowId] = state;
}

WindowDeviceState StateCache::GetWindow(int windowId)
{
    auto it = _windows.find(windowId);

    if (it != _windows.end())
        return it->second;

    return {};
}

bool StateCache::HasWindow(int windowId)
{
    return _windows.find(windowId) != _windows.end();
}

//---------------- Cameras ----------------

void StateCache::UpdateCamera(const CameraDeviceState& state)
{
    _cameras[state.CameraId] = state;
}

CameraDeviceState StateCache::GetCamera(int cameraId)
{
    auto it = _cameras.find(cameraId);

    if (it != _cameras.end())
        return it->second;

    return {};
}

bool StateCache::HasCamera(int cameraId)
{
    return _cameras.find(cameraId) != _cameras.end();
}

//---------------- Externals ----------------

void StateCache::UpdateExternal(const ExternalDeviceState& state)
{
    _externals[state.ExternalId] = state;
}

ExternalDeviceState StateCache::GetExternal(int externalId)
{
    auto it = _externals.find(externalId);

    if (it != _externals.end())
        return it->second;

    return {};
}

bool StateCache::HasExternal(int externalId)
{
    return _externals.find(externalId) != _externals.end();
}