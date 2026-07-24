#pragma once

#include "HardwareModel.h"

class HardwareManager
{
public:

    void Initialize();

    bool RegisterLight(int id, uint8_t address, bool pwm = false);
    bool RegisterAppliance(int id, uint8_t address);
    bool RegisterDoor(int id, uint8_t address);
    bool RegisterWindow(int id, uint8_t address);
    bool RegisterCamera(int id, uint8_t address);
    bool RegisterExternal(int id, uint8_t address);

    HardwareDevice* GetLight(int id);
    HardwareDevice* GetAppliance(int id);
    HardwareDevice* GetDoor(int id);
    HardwareDevice* GetWindow(int id);
    HardwareDevice* GetCamera(int id);
    HardwareDevice* GetExternal(int id);

private:

    HardwareDevice _lights[MAX_LIGHTS];

    HardwareDevice _appliances[MAX_APPLIANCES];

    HardwareDevice _doors[MAX_DOORS];

    HardwareDevice _windows[MAX_WINDOWS];

    HardwareDevice _cameras[MAX_CAMERAS];

    HardwareDevice _externals[MAX_EXTERNALS];
};