#include "Light.h"
void LightManager::Apply(const LightDeviceState& state)
{
    auto current = GetCurrentState(state.LightId);

    if(current.IsActive == state.IsActive &&
       current.BrightnessLevel == state.BrightnessLevel)
    {
        return;
    }

    SetRelay(state.LightId, state.IsActive);

    SetBrightness(state.LightId, state.BrightnessLevel);

    _cache[state.LightId] = state;
}
void LightManager::Update(const GenericDeviceUpdateState& update)
{
    LightDeviceState state;

    state.LightId = update.Id;

    state.IsActive = update.IsActive;

    state.BrightnessLevel = update.BrightnessLevel;

    Apply(state);
}
LightDeviceState LightManager::GetCurrentState(int id)
{
    return _cache[id];
}