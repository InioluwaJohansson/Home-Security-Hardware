#pragma once

#include "DeviceModels.h"
#include "FacilityLimits.h"
#include "../Hardware/IHardwareController.h"
#include <functional>
class ApplianceManager
{
public:

    std::function<void(const GenericDeviceUpdateState&)> StateChanged;
    
    void Initialize();

    void Apply(const ApplianceDeviceState& state);

    void Update(const GenericDeviceUpdateState& update);

    ApplianceDeviceState GetCurrentState(int applianceId);
    
private:
    IHardwareController& _hardware;

    ApplianceDeviceState _cache[MAX_APPLIANCES];
};