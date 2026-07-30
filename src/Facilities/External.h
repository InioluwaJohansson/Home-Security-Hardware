#pragma once

#include "DeviceModels.h"
#include "../Hardware/IHardwareController.h"
#include <functional>
class ExternalManager
{
public:

    std::function<void(const GenericDeviceUpdateState&)> StateChanged;
    
    void Initialize();
    
    void Apply(const ExternalDeviceState& state);
    
    void Update(const GenericDeviceUpdateState& update);
    
    ExternalDeviceState GetCurrentState(int externalId);
    
private:

    ExternalDeviceState _cache[MAX_EXTERNALS];
    
    IHardwareController& _hardware;
};