#pragma once

#include <Arduino.h>
#include <functional>

#include "DeviceModels.h"
#include "FacilityLimits.h"
#include "../Hardware/IHardwareController.h"

class LightManager
{
public:

    LightManager(
        IHardwareController& hardware);

    std::function<void(
        const GenericDeviceUpdateState&)>
        StateChanged;

    void Initialize();

    void Apply(
        const LightDeviceState& state);

    void Update(
        const GenericDeviceUpdateState& update);

    LightDeviceState GetCurrentState(
        int lightId);

private:

    LightDeviceState _cache[MAX_LIGHTS];

    IHardwareController& _hardware;
};