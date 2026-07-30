#include "Light.h"
#include "../Hardware/Hardware.h"

LightManager::LightManager(
    IHardwareController& hardware)
    : _hardware(hardware)
{
}

void LightManager::Initialize()
{
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        _cache[i].LightId = -1;
        _cache[i].IsActive = false;
        _cache[i].BrightnessLevel = 0;
    }
}

LightDeviceState LightManager::GetCurrentState(int lightId)
{
    return _cache[lightId];
}
void LightManager::Apply(const LightDeviceState& state)
{
    auto current = GetCurrentState(state.LightId);

    if(current.IsActive == state.IsActive &&
       current.BrightnessLevel == state.BrightnessLevel)
    {
        return;
    }

    _hardware.SetLight(
        state.LightId,
        state.IsActive);

    _hardware.SetBrightness(
        state.LightId,
        state.BrightnessLevel);

    _cache[state.LightId] = state;

    if(StateChanged)
    {
        GenericDeviceUpdateState update;

        update.Id = state.LightId;
        update.Type = FacilityType::Light;
        update.IsActive = state.IsActive;
        update.BrightnessLevel = state.BrightnessLevel;

        StateChanged(update);
    }
}
void LightManager::Update(const GenericDeviceUpdateState& update)
{
    LightDeviceState state;

    state.LightId = update.Id;
    state.IsActive = update.IsActive;
    state.BrightnessLevel = update.BrightnessLevel;

    Apply(state);
}