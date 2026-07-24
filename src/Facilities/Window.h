#pragma once

#include "DeviceModels.h"
#include "../Hardware/IHardwareController.h"
class WindowManager
{
    public:
    
    void Initialize();
    
    void Apply(const WindowDeviceState& state);
    
    void Update(const GenericDeviceUpdateState& update);
    
    WindowDeviceState GetCurrentState(int windowId);
    
private:
    
    WindowDeviceState _cache[MAX_WINDOWS];

    IHardwareController& _hardware;
};