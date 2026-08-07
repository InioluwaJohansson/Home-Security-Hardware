#pragma once

#include "DeviceModels.h"
#include "FacilityLimits.h"
#include "../Hardware/IHardwareController.h"
#include <functional>
class LightManager
{
public:

    std::function<void(const GenericDeviceUpdateState&)> StateChanged;
    
    void Initialize();

    void Apply(const LightDeviceState& state);

    void Update(const GenericDeviceUpdateState& update);

    LightDeviceState GetCurrentState(int lightId);
    
private:
    
    LightDeviceState _cache[MAX_LIGHTS];

    IHardwareController& _hardware;

};