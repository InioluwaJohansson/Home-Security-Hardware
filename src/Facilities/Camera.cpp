#include "Camera.h"
#include "../Hardware/Hardware.h"
CameraManager::CameraManager(
    IHardwareController& hardware)
    : _hardware(hardware)
{
}
void CameraManager::Initialize()
{
    for (int i = 0; i < MAX_CAMERAS; i++)
    {
        _cache[i].CameraId = -1;
        _cache[i].IsActive = false;
    }
}

CameraDeviceState CameraManager::GetCurrentState(int cameraId)
{
    return _cache[cameraId];
}

void CameraManager::Apply(const CameraDeviceState& state)
{
    auto current = GetCurrentState(state.CameraId);

    if (current.IsActive == state.IsActive)
        return;

    _hardware.SetCamera(state.CameraId, state.IsActive);

    _cache[state.CameraId] = state;
    if(StateChanged)
    {
        GenericDeviceUpdateState update;

        update.Id = state.CameraId;
        update.Type = FacilityType::Camera;
        update.IsActive = state.IsActive;

        StateChanged(update);
    }
}

void CameraManager::Update(const GenericDeviceUpdateState& update)
{
    CameraDeviceState state;

    state.CameraId = update.Id;
    state.IsActive = update.IsActive;

    Apply(state);
}