#include "Appliance.h"
#include "../Hardware/Hardware.h"
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
    return _cache[applianceId];
}

void ApplianceManager::Apply(const ApplianceDeviceState& state)
{
    auto current = GetCurrentState(state.ApplianceId);

    if (current.IsActive == state.IsActive)
        return;

    _hardware.SetAppliance(state.ApplianceId, state.IsActive);

    _cache[state.ApplianceId] = state;
}

void ApplianceManager::Update(const GenericDeviceUpdateState& update)
{
    ApplianceDeviceState state;

    state.ApplianceId = update.Id;
    state.IsActive = update.IsActive;

    Apply(state);
}