#pragma once

#include "DeviceModels.h"
#include "../Hardware/IHardwareController.h"
class DoorManager
{
public:
    
    void Initialize();
    
    void Apply(const DoorDeviceState& state);
    
    void Update(const GenericDeviceUpdateState& update);
    
    DoorDeviceState GetCurrentState(int doorId);
    
private:
    
    DoorDeviceState _cache[MAX_DOORS];

    IHardwareController& _hardware;
};