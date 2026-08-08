#include "Sensor.h"
#include "../Facilities/FacilityLimits.h"
SensorController::SensorController(
    IHardwareController& hardware,
    ConfigurationManager& configuration,
    DeviceManager& devices,
    StateCache& cache,
    MqttManager& mqtt)
    :
    _hardware(hardware),
    _configuration(configuration),
    _devices(devices),
    _cache(cache),
    _mqtt(mqtt)
{
}

void SensorController::Initialize()
{
    _cache.Initialize();
}

void SensorController::Update()
{
    static unsigned long lastPoll = 0;
    constexpr unsigned long PollInterval = 20;

    const unsigned long now = millis();

    if (now - lastPoll < PollInterval)
        return;

    lastPoll = now;

    UpdateDoors();
    UpdateWindows();
    UpdateExternals();
    UpdateResetButton();
}

void SensorController::UpdateDoors()
{
    for (int i = 0; i < MAX_DOORS; ++i)
    {
        DoorDeviceState state;

        if (_cache.HasDoor(i))
            state = _cache.GetDoor(i);
        else
            state.DoorId = i;

        const bool closed = _hardware.ReadDoorClosedSensor(i);
        const bool locked = _hardware.ReadDoorLockedSensor(i);

        if (state.IsOpen == !closed &&
            state.IsLocked == locked)
        {
            continue;
        }

        state.DoorId = i;
        state.IsOpen = !closed;
        state.IsLocked = locked;
        // DoorType is configuration, not a physical sensor value. Preserve
        // the cached backend profile while updating only physical state.
        if (state.Type != DoorType::Interior &&
            state.Type != DoorType::Exterior &&
            state.Type != DoorType::Gate)
        {
            state.Type = DoorType::Interior;
        }

        _cache.UpdateDoor(state);

        JsonDocument json;

        json["doorId"] = state.DoorId;
        json["isOpen"] = state.IsOpen;
        json["isLocked"] = state.IsLocked;

        _mqtt.Publish(
            "home/" +
            _configuration.GetConfiguration().HardwareId +
            "/state",
            json);
    }
}

void SensorController::UpdateWindows()
{
    for (int i = 0; i < MAX_WINDOWS; ++i)
    {
        WindowDeviceState state;

        if (_cache.HasWindow(i))
            state = _cache.GetWindow(i);
        else
            state.WindowId = i;

        const bool closed = _hardware.ReadWindowClosedSensor(i);
        const bool locked = _hardware.ReadWindowLockedSensor(i);

        if (state.IsOpen == !closed &&
            state.IsLocked == locked)
        {
            continue;
        }

        state.WindowId = i;
        state.IsOpen = !closed;
        state.IsLocked = locked;

        _cache.UpdateWindow(state);

        JsonDocument json;

        json["windowId"] = state.WindowId;
        json["isOpen"] = state.IsOpen;
        json["isLocked"] = state.IsLocked;

        _mqtt.Publish(
            "home/" +
            _configuration.GetConfiguration().HardwareId +
            "/state",
            json);
    }
}

void SensorController::UpdateExternals()
{
    for (int i = 0; i < MAX_EXTERNALS; ++i)
    {
        // IMPORTANT:
        // IsActive is the enable/configuration flag.
        // If the external input is disabled, do NOT read the sensor.
        if (!_cache.HasExternal(i))
            continue;

        ExternalDeviceState state = _cache.GetExternal(i);

        if (!state.IsActive)
            continue;

        const bool triggered =
            _hardware.ReadExternalSensor(i);

        if (state.IsTriggered == triggered)
            continue;

        state.IsTriggered = triggered;

        _cache.UpdateExternal(state);

        JsonDocument json;

        json["externalId"] = state.ExternalId;
        json["isActive"] = state.IsActive;
        json["isTriggered"] = state.IsTriggered;

        _mqtt.Publish(
            "home/" +
            _configuration.GetConfiguration().HardwareId +
            "/state",
            json);
    }
}

void SensorController::UpdateResetButton()
{
    static bool pressed = false;
    static unsigned long pressedTime = 0;

    const bool current = _hardware.ReadResetButton();

    if (current)
    {
        if (!pressed)
        {
            pressed = true;
            pressedTime = millis();
        }

        if (millis() - pressedTime >= 10000)
        {
            pressed = false;
            // Reset/provisioning action can be wired here.
        }
    }
    else
    {
        pressed = false;
    }
}
