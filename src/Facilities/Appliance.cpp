#include "Appliance.h"
ApplianceManager::ApplianceManager(
    IHardwareController& hardware)
    : _hardware(hardware)
{
}
void ApplianceManager::Initialize()
{
    for (int i = 0; i < MAX_APPLIANCES; i++)
    {
        _cache[i].ApplianceId = -1;
        _cache[i].IsActive = false;
    }
}

ApplianceDeviceState ApplianceManager::GetCurrentState(int applianceId)
{
    if (applianceId < 0 || applianceId >= MAX_APPLIANCES) return {};
    return _cache[applianceId];
}

void ApplianceManager::Apply(const ApplianceDeviceState& state)
{
    if (state.ApplianceId < 0 || state.ApplianceId >= MAX_APPLIANCES) return;

    auto current = GetCurrentState(state.ApplianceId);

    if (current.IsActive == state.IsActive)
        return;

    _hardware.SetAppliance(state.ApplianceId, state.IsActive);

    _cache[state.ApplianceId] = state;
    if(StateChanged)
    {
        GenericDeviceUpdateState update;

        update.Id = state.ApplianceId;
        update.Type = FacilityType::Appliance;
        update.IsActive = state.IsActive;

        StateChanged(update);
    }
}

void ApplianceManager::Update(const GenericDeviceUpdateState& update)
{
    ApplianceDeviceState state;

    state.ApplianceId = update.Id;
    state.IsActive = update.IsActive;

    Apply(state);
}