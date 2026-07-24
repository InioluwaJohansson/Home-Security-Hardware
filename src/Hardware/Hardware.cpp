#include "Hardware.h"

void HardwareManager::Initialize()
{
}

template<size_t N>
static bool RegisterDevice(
    HardwareDevice (&devices)[N],
    FacilityType type,
    int id,
    uint8_t address,
    bool pwm = false)
{
    for (auto& device : devices)
    {
        if (!device.Enabled)
        {
            device.Id = id;
            device.Type = type;
            device.Address = address;
            device.Enabled = true;
            device.SupportsPWM = pwm;

            return true;
        }
    }

    return false;
}

template<size_t N>
static HardwareDevice* FindDevice(
    HardwareDevice (&devices)[N],
    int id)
{
    for (auto& device : devices)
    {
        if (device.Enabled &&
            device.Id == id)
        {
            return &device;
        }
    }

    return nullptr;
}

//---------------- Lights ----------------

bool HardwareManager::RegisterLight(
    int id,
    uint8_t address,
    bool pwm)
{
    return RegisterDevice(
        _lights,
        FacilityType::Light,
        id,
        address,
        pwm);
}

HardwareDevice* HardwareManager::GetLight(int id)
{
    return FindDevice(_lights, id);
}

//---------------- Appliances ----------------

bool HardwareManager::RegisterAppliance(
    int id,
    uint8_t address)
{
    return RegisterDevice(
        _appliances,
        FacilityType::Appliance,
        id,
        address);
}

HardwareDevice* HardwareManager::GetAppliance(int id)
{
    return FindDevice(_appliances, id);
}

//---------------- Doors ----------------

bool HardwareManager::RegisterDoor(
    int id,
    uint8_t address)
{
    return RegisterDevice(
        _doors,
        FacilityType::Door,
        id,
        address);
}

HardwareDevice* HardwareManager::GetDoor(int id)
{
    return FindDevice(_doors, id);
}

//---------------- Windows ----------------

bool HardwareManager::RegisterWindow(
    int id,
    uint8_t address)
{
    return RegisterDevice(
        _windows,
        FacilityType::Window,
        id,
        address);
}

HardwareDevice* HardwareManager::GetWindow(int id)
{
    return FindDevice(_windows, id);
}

//---------------- Cameras ----------------

bool HardwareManager::RegisterCamera(
    int id,
    uint8_t address)
{
    return RegisterDevice(
        _cameras,
        FacilityType::Camera,
        id,
        address);
}

HardwareDevice* HardwareManager::GetCamera(int id)
{
    return FindDevice(_cameras, id);
}

//---------------- Externals ----------------

bool HardwareManager::RegisterExternal(
    int id,
    uint8_t address)
{
    return RegisterDevice(
        _externals,
        FacilityType::External,
        id,
        address);
}

HardwareDevice* HardwareManager::GetExternal(int id)
{
    return FindDevice(_externals, id);
}