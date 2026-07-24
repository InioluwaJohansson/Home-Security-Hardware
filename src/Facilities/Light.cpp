#include "Light.h"

LightManager::LightManager(
    IHardwareController& hardware)
    : _hardware(hardware)
{
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
}