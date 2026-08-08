#pragma once

#include "HardwareModel.h"
#include "../Facilities/FacilityLimits.h"

class HardwareManager
{
public:
    void Initialize();

    bool RegisterLight(int id);
    bool RegisterAppliance(int id);
    bool RegisterDoor(int id);
    bool RegisterWindow(int id);
    bool RegisterCamera(int id);
    bool RegisterExternal(int id);

    HardwareDevice* GetLight(int id);
    HardwareDevice* GetAppliance(int id);
    HardwareDevice* GetDoor(int id);
    HardwareDevice* GetDoorSlot(uint8_t slot);
    HardwareDevice* GetWindow(int id);
    HardwareDevice* GetWindowSlot(uint8_t slot);
    HardwareDevice* GetCamera(int id);
    HardwareDevice* GetExternal(int id);

private:
    HardwareDevice _lights[MAX_LIGHTS];
    HardwareDevice _appliances[MAX_APPLIANCES];
    HardwareDevice _doors[MAX_DOORS];
    HardwareDevice _windows[MAX_WINDOWS];
    HardwareDevice _cameras[MAX_CAMERAS];
    HardwareDevice _externals[MAX_EXTERNALS];

    void ConfigureFixedResources();

    template <size_t N>
    HardwareDevice* Register(
        HardwareDevice (&devices)[N],
        FacilityType type,
        int id);

    template <size_t N>
    HardwareDevice* Find(
        HardwareDevice (&devices)[N],
        int id);
};
