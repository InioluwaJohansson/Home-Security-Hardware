#pragma once

#include "DeviceModels.h"
#include "FacilityLimits.h"
#include "../Hardware/IHardwareController.h"
#include <functional>
class WindowManager
{
public:

    std::function<void(const GenericDeviceUpdateState&)> StateChanged;
    
    void Initialize();
    
    void Apply(const WindowDeviceState& state);
    
    void Update(const GenericDeviceUpdateState& update);
    
    WindowDeviceState GetCurrentState(int windowId);
    
private:
    
    WindowDeviceState _cache[MAX_WINDOWS];

    IHardwareController& _hardware;
};