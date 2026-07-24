#pragma once

#include "DeviceModels.h"
#include "../Hardware/IHardwareController.h"

class LightManager
{
public:

    void Initialize();

    void Apply(const LightDeviceState& state);

    void Update(const GenericDeviceUpdateState& update);

    LightDeviceState GetCurrentState(int lightId);
    
private:
    
    LightDeviceState _cache[MAX_LIGHTS];

    IHardwareController& _hardware;

};