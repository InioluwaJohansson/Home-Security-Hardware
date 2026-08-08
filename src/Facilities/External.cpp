#include "External.h"

ExternalManager::ExternalManager(
    IHardwareController& hardware)
    : _hardware(hardware)
{
}

void ExternalManager::Initialize()
{
    for (int i = 0; i < MAX_EXTERNALS; ++i)
    {
        _cache[i].ExternalId = i;
        _cache[i].IsActive = false;
        _cache[i].IsTriggered = false;
    }
}

ExternalDeviceState ExternalManager::GetCurrentState(int externalId)
{
    if (externalId < 0 || externalId >= MAX_EXTERNALS)
        return {};

    return _cache[externalId];
}

void ExternalManager::Apply(const ExternalDeviceState& state)
{
    if (state.ExternalId < 0 || state.ExternalId >= MAX_EXTERNALS)
        return;

    const auto current = _cache[state.ExternalId];

    _cache[state.ExternalId] = state;

    // External inputs are sensors. Do not read or write the hardware here.
    // SensorController owns polling and obeys IsActive.
    if (StateChanged &&
        (current.IsActive != state.IsActive ||
         current.IsTriggered != state.IsTriggered))
    {
        GenericDeviceUpdateState update;

        update.Id = state.ExternalId;
        update.Type = FacilityType::External;
        update.IsTriggered = state.IsTriggered;
        update.IsActive = state.IsActive;

        StateChanged(update);
    }
}

void ExternalManager::Update(const GenericDeviceUpdateState& update)
{
    ExternalDeviceState state;

    state.ExternalId = update.Id;
    state.IsActive = update.IsActive;
    state.IsTriggered = update.IsTriggered;

    Apply(state);
}
