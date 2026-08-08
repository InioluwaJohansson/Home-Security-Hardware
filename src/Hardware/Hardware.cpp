#include "Hardware.h"

namespace
{
    constexpr uint8_t MCP_LIGHTS = 0;
    constexpr uint8_t MCP_DOOR_DIRECTION = 1;
    constexpr uint8_t MCP_WINDOW_DIRECTION = 2;
    constexpr uint8_t MCP_ENCODERS = 3;

    constexpr uint8_t PCA_MOTOR = 0;
    constexpr uint8_t PCA_LIGHT = 1;

    constexpr uint8_t ADC_DOOR_MOTOR = 0;
    constexpr uint8_t ADC_WINDOW_MOTOR = 1;
    constexpr uint8_t ADC_DOOR_LOCK = 2;
    constexpr uint8_t ADC_WINDOW_LOCK = 3;

    constexpr SensorBus MUX_DOOR = SensorBus::DoorMux;
    constexpr SensorBus MUX_WINDOW = SensorBus::WindowMux;
    constexpr SensorBus MUX_EXTERNAL = SensorBus::ExternalMux;

    constexpr float DEFAULT_CURRENT_ZERO = 2.5f;
    constexpr float DEFAULT_CURRENT_SENSITIVITY = 0.040f; // Placeholder for a typical 50A Hall sensor; calibrate to the selected sensor.

    DigitalEndpoint Digital(uint8_t chip, uint8_t pin)
    {
        return { chip, pin };
    }

    PwmEndpoint Pwm(uint8_t controller, uint8_t channel)
    {
        return { controller, channel };
    }

    SensorEndpoint Sensor(SensorBus bus, uint8_t channel, bool activeHigh = true)
    {
        return { bus, channel, activeHigh };
    }

    CurrentSensorEndpoint Current(
        uint8_t adc,
        uint8_t channel,
        float zeroVoltage = DEFAULT_CURRENT_ZERO,
        float voltsPerAmp = DEFAULT_CURRENT_SENSITIVITY)
    {
        return { adc, channel, zeroVoltage, voltsPerAmp };
    }

    void ConfigureMotor(
        HardwareDevice& device,
        uint8_t index,
        bool window)
    {
        const SensorBus mux = window ? MUX_WINDOW : MUX_DOOR;
        const uint8_t directionChip = window ? MCP_WINDOW_DIRECTION : MCP_DOOR_DIRECTION;
        const uint8_t directionBase = window ? 0 : 1;
        const uint8_t pwmBase = window ? 8 : 0;
        const uint8_t lockPwmBase = window ? 12 : 4;
        const uint8_t adcMotor = window ? ADC_WINDOW_MOTOR : ADC_DOOR_MOTOR;
        const uint8_t adcLock = window ? ADC_WINDOW_LOCK : ADC_DOOR_LOCK;

        // Each motorized facility gets exactly three position inputs:
        //   channel 0 + index*3 = fully open Hall sensor
        //   channel 1 + index*3 = fully closed Hall sensor
        //   channel 2 + index*3 = lock/unlock position sensor
        // Four facilities therefore use channels 0..11, which is within the
        // 16 channels available on a single 74HC4067.
        const uint8_t sensorBase = static_cast<uint8_t>(index * 3);

        device.motorized.motor.pwm = Pwm(PCA_MOTOR, pwmBase + index);
        device.motorized.motor.direction = Digital(directionChip, directionBase + index);

        const uint8_t encoderBase = window
            ? static_cast<uint8_t>(8 + index * 2)
            : static_cast<uint8_t>(index * 2);

        device.motorized.motor.encoderA = Sensor(SensorBus::McuGpio, encoderBase);
        device.motorized.motor.encoderB = Sensor(SensorBus::McuGpio, encoderBase + 1);

        device.motorized.motor.openSensor = Sensor(mux, sensorBase);
        device.motorized.motor.closeSensor = Sensor(mux, sensorBase + 1);
        device.motorized.lock.positionSensor = Sensor(mux, sensorBase + 2);

        device.motorized.motor.current = Current(adcMotor, index);
        device.motorized.lock.current = Current(adcLock, index);

        device.motorized.lock.pwm = Pwm(PCA_MOTOR, lockPwmBase + index);
        device.motorized.lock.direction = Digital(directionChip, (window ? 4 : 5) + index);

        // Windows and interior/gate doors use a moderate default profile.
        // Exterior doors are raised to a faster profile when DoorType is set.
        device.motorized.motor.maxSpeed = window ? 65 : 65;
        device.motorized.motor.acceleration = window ? 20 : 25;
        device.motorized.motor.deceleration = window ? 20 : 25;
        device.motorized.motor.maxCurrent = window ? 5000 : 10000;
        device.motorized.motor.maxTravelTime = window ? 20000 : 30000;
        device.motorized.lock.maxCurrent = 3000;
        device.motorized.lock.maxTravelTime = 5000;
    }
}

void HardwareManager::Initialize()
{
    for (auto& device : _lights) device = {};
    for (auto& device : _appliances) device = {};
    for (auto& device : _doors) device = {};
    for (auto& device : _windows) device = {};
    for (auto& device : _cameras) device = {};
    for (auto& device : _externals) device = {};

    ConfigureFixedResources();
}

void HardwareManager::ConfigureFixedResources()
{
    // Lights: MCP #0 pins 0-5, PCA #1 channels 0-5.
    for (uint8_t i = 0; i < MAX_LIGHTS; ++i)
    {
        _lights[i].id = i;
        _lights[i].type = FacilityType::Light;
        _lights[i].enabled = true;
        _lights[i].primary = Digital(MCP_LIGHTS, i);
        _lights[i].pwm = Pwm(PCA_LIGHT, i);
    }

    // Appliances: MCP #0 pins 6-13.
    for (uint8_t i = 0; i < MAX_APPLIANCES; ++i)
    {
        _appliances[i].id = i;
        _appliances[i].type = FacilityType::Appliance;
        _appliances[i].enabled = true;
        _appliances[i].primary = Digital(MCP_LIGHTS, 6 + i);
    }

    // Cameras: MCP #0 pins 14-15 and MCP #1 pin 0.
    for (uint8_t i = 0; i < MAX_CAMERAS; ++i)
    {
        _cameras[i].id = i;
        _cameras[i].type = FacilityType::Camera;
        _cameras[i].enabled = true;
        _cameras[i].primary = i < 2
            ? Digital(MCP_LIGHTS, 14 + i)
            : Digital(MCP_DOOR_DIRECTION, 0);
    }

    // Doors: MCP #1 pins 1-4 movement direction, 5-8 lock direction.
    for (uint8_t i = 0; i < MAX_DOORS; ++i)
    {
        _doors[i].id = i;
        _doors[i].type = FacilityType::Door;
        _doors[i].enabled = true;
        _doors[i].doorType = DoorType::Interior;
        ConfigureMotor(_doors[i], i, false);
    }

    // Windows: MCP #2 pins 0-3 movement direction, 4-7 lock direction.
    for (uint8_t i = 0; i < MAX_WINDOWS; ++i)
    {
        _windows[i].id = i;
        _windows[i].type = FacilityType::Window;
        _windows[i].enabled = true;
        ConfigureMotor(_windows[i], i, true);
    }

    // External outputs are MCP #2 pins 8-12. External sensor inputs use MUX #2 channels 0-4.
    for (uint8_t i = 0; i < MAX_EXTERNALS; ++i)
    {
        _externals[i].id = i;
        _externals[i].type = FacilityType::External;
        _externals[i].enabled = true;
        _externals[i].primary = Digital(MCP_WINDOW_DIRECTION, 8 + i);
        _externals[i].sensor = Sensor(MUX_EXTERNAL, i);
    }
}

template <size_t N>
HardwareDevice* HardwareManager::Register(
    HardwareDevice (&devices)[N],
    FacilityType type,
    int id)
{
    if (id < 0)
        return nullptr;

    for (auto& device : devices)
    {
        if (device.enabled && device.id == id)
            return &device;
    }

    if (static_cast<size_t>(id) < N)
    {
        auto& device = devices[id];
        if (!device.enabled)
        {
            device.id = id;
            device.type = type;
            device.enabled = true;
            return &device;
        }
    }

    return nullptr;
}

template <size_t N>
HardwareDevice* HardwareManager::Find(
    HardwareDevice (&devices)[N],
    int id)
{
    for (auto& device : devices)
    {
        if (device.enabled && device.id == id)
            return &device;
    }

    return nullptr;
}

bool HardwareManager::RegisterLight(int id)
{
    return Register(_lights, FacilityType::Light, id) != nullptr;
}

bool HardwareManager::RegisterAppliance(int id)
{
    return Register(_appliances, FacilityType::Appliance, id) != nullptr;
}

bool HardwareManager::RegisterDoor(int id)
{
    return Register(_doors, FacilityType::Door, id) != nullptr;
}

bool HardwareManager::RegisterWindow(int id)
{
    return Register(_windows, FacilityType::Window, id) != nullptr;
}

bool HardwareManager::RegisterCamera(int id)
{
    return Register(_cameras, FacilityType::Camera, id) != nullptr;
}

bool HardwareManager::RegisterExternal(int id)
{
    return Register(_externals, FacilityType::External, id) != nullptr;
}

HardwareDevice* HardwareManager::GetLight(int id) { return Find(_lights, id); }
HardwareDevice* HardwareManager::GetAppliance(int id) { return Find(_appliances, id); }
HardwareDevice* HardwareManager::GetDoor(int id) { return Find(_doors, id); }
HardwareDevice* HardwareManager::GetDoorSlot(uint8_t slot)
{
    return slot < MAX_DOORS ? &_doors[slot] : nullptr;
}
HardwareDevice* HardwareManager::GetWindow(int id) { return Find(_windows, id); }
HardwareDevice* HardwareManager::GetWindowSlot(uint8_t slot)
{
    return slot < MAX_WINDOWS ? &_windows[slot] : nullptr;
}
HardwareDevice* HardwareManager::GetCamera(int id) { return Find(_cameras, id); }
HardwareDevice* HardwareManager::GetExternal(int id) { return Find(_externals, id); }
