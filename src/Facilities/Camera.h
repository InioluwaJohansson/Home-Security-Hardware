#pragma once

#include "DeviceModels.h"
#include "FacilityLimits.h"
#include "../Hardware/IHardwareController.h"
#include <functional>
class CameraManager
{
public:

    std::function<void(const GenericDeviceUpdateState&)> StateChanged;

    void Initialize();
    
    void Apply(const CameraDeviceState& state);
    
    void Update(const GenericDeviceUpdateState& update);
    
    CameraDeviceState GetCurrentState(int cameraId);
    
private:
    
    CameraDeviceState _cache[MAX_CAMERAS];
    
    IHardwareController& _hardware;
};