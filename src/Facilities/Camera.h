#pragma once

#include "DeviceModels.h"
#include "../Hardware/IHardwareController.h"
class CameraManager
{
public:
    
    void Initialize();
    
    void Apply(const CameraDeviceState& state);
    
    void Update(const GenericDeviceUpdateState& update);
    
    CameraDeviceState GetCurrentState(int cameraId);
    
private:
    
    CameraDeviceState _cache[MAX_CAMERAS];
    
    IHardwareController& _hardware;
};