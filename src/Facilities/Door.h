#pragma once

#include "DeviceModels.h"
#include "FacilityLimits.h"
#include "../Hardware/IHardwareController.h"
#include <functional>
class DoorManager
{
public:

    std::function<void(const GenericDeviceUpdateState&)> StateChanged;
    
    void Initialize();
    
    void Apply(const DoorDeviceState& state);
    
    void Update(const GenericDeviceUpdateState& update);
    
    DoorDeviceState GetCurrentState(int doorId);
    
private:
    
    DoorDeviceState _cache[MAX_DOORS];

    IHardwareController& _hardware;
};