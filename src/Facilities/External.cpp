#include "External.h"
#include "../Hardware/Hardware.h"

ExternalManager::ExternalManager(
    IHardwareController& hardware)
    : _hardware(hardware)
{
}
void ExternalManager::Initialize()
{
    for (int i = 0; i < MAX_EXTERNALS; i++)
    {
        _cache[i].ExternalId = -1;
        _cache[i].IsActive = false;
        _cache[i].IsTriggered = false;
    }
}

ExternalDeviceState ExternalManager::GetCurrentState(int externalId)
{
    return _cache[externalId];
}

void ExternalManager::Apply(const ExternalDeviceState& state)
{
    auto current = GetCurrentState(state.ExternalId);

    if (current.IsActive == state.IsActive &&
        current.IsTriggered == state.IsTriggered){
        
            _hardware.SetExternal(state.ExternalId, state.IsActive);

        _cache[state.ExternalId] = state;
        return;}

    // Input device.
    // No hardware output required.

    _cache[state.ExternalId] = state;
}

void ExternalManager::Update(const GenericDeviceUpdateState& update)
{
    ExternalDeviceState state;

    state.ExternalId = update.Id;
    state.IsActive = update.IsActive;
    state.IsTriggered = update.IsTriggered;

    Apply(state);
}