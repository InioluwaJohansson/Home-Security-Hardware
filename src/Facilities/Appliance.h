#pragma once

#include "DeviceModels.h"
#include "../Hardware/IHardwareController.h"
class ApplianceManager
{
public:

    void Initialize();

    void Apply(const ApplianceDeviceState& state);

    void Update(const GenericDeviceUpdateState& update);

    ApplianceDeviceState GetCurrentState(int applianceId);
    
private:
    IHardwareController& _hardware;

    ApplianceDeviceState _cache[MAX_APPLIANCES];
};