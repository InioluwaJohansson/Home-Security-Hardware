#pragma once

#include "DeviceModels.h"
#include "../Hardware/IHardwareController.h"
class ExternalManager
{
public:
    
    void Initialize();
    
    void Apply(const ExternalDeviceState& state);
    
    void Update(const GenericDeviceUpdateState& update);
    
    ExternalDeviceState GetCurrentState(int externalId);
    
private:

    ExternalDeviceState _cache[MAX_EXTERNALS];
    
    IHardwareController& _hardware;
};