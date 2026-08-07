#pragma once

#include <Arduino.h>
#include <functional>

#include "DeviceModels.h"
#include "FacilityLimits.h"
#include "../Hardware/IHardwareController.h"

class ExternalManager
{
public:

    ExternalManager(
        IHardwareController& hardware);

    std::function<void(
        const GenericDeviceUpdateState&)>
        StateChanged;

    void Initialize();

    void Apply(
        const ExternalDeviceState& state);

    void Update(
        const GenericDeviceUpdateState& update);

    ExternalDeviceState GetCurrentState(
        int externalId);

private:

    ExternalDeviceState _cache[MAX_EXTERNALS];

    IHardwareController& _hardware;
};