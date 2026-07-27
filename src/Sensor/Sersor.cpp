#include "Sensor.h"

SensorController::SensorController(
    IHardwareController& hardware,
    DeviceManager& devices,
    StateCache& cache,
    MqttManager& mqtt)
    :
    _hardware(hardware),
    _devices(devices),
    _cache(cache),
    _mqtt(mqtt)
{
}

void SensorController::Initialize()
{
}

void SensorController::Update()
{
    static unsigned long lastPoll = 0;

    constexpr unsigned long PollInterval = 20;

    unsigned long now = millis();

    if (now - lastPoll < PollInterval)
    {
        return;
    }

    lastPoll = now;

    UpdateDoors();

    UpdateWindows();

    UpdateExternals();

    UpdateResetButton();
}

void SensorController::UpdateDoors()
{
    for (int i = 0; i < MAX_DOORS; i++)
    {
        DoorDeviceState state;

        if (_cache.HasDoor(i))
        {
            state = _cache.GetDoor(i);
        }

        bool closed = _hardware.ReadDoorClosedSensor(i);

        bool locked = _hardware.ReadDoorLockedSensor(i);

        if (state.IsClosed == closed &&
            state.IsLocked == locked)
        {
            continue;
        }

        state.DoorId = i;

        state.IsClosed = closed;

        state.IsLocked = locked;

        _cache.UpdateDoor(state);

        StaticJsonDocument<256> json;

        json["doorId"] = state.DoorId;
        json["isClosed"] = state.IsClosed;
        json["isLocked"] = state.IsLocked;

        _mqtt.Publish(
            "home/" +
            _devices.GetHardwareId() +
            "/door/state",
            json);
    }
}

void SensorController::UpdateWindows()
{
    for (int i = 0; i < MAX_WINDOWS; i++)
    {
        WindowDeviceState state;

        if (_cache.HasWindow(i))
        {
            state = _cache.GetWindow(i);
        }

        bool closed = _hardware.ReadWindowClosedSensor(i);

        bool locked = _hardware.ReadWindowLockedSensor(i);

        if (state.IsClosed == closed &&
            state.IsLocked == locked)
        {
            continue;
        }

        state.WindowId = i;

        state.IsClosed = closed;

        state.IsLocked = locked;

        _cache.UpdateWindow(state);

        StaticJsonDocument<256> json;

        json["windowId"] = state.WindowId;
        json["isClosed"] = state.IsClosed;
        json["isLocked"] = state.IsLocked;

        _mqtt.Publish(
            "home/" +
            _devices.GetHardwareId() +
            "/window/state",
            json);
    }
}

void SensorController::UpdateExternals()
{
    for (int i = 0; i < MAX_EXTERNALS; i++)
    {
        ExternalDeviceState state;

        if (_cache.HasExternal(i))
        {
            state = _cache.GetExternal(i);
        }

        bool motion = _hardware.ReadExternalSensor(i);

        if (state.IsActive == motion)
        {
            continue;
        }

        state.ExternalId = i;

        state.IsActive = motion;

        _cache.UpdateExternal(state);

        StaticJsonDocument<256> json;

        json["externalId"] = state.ExternalId;
        json["motion"] = state.IsActive;

        _mqtt.Publish(
            "home/" +
            _devices.GetHardwareId() +
            "/external/state",
            json);
    }
}

void SensorController::UpdateResetButton()
{
    static bool pressed = false;

    static unsigned long pressedTime = 0;

    bool current = _hardware.ReadResetButton();

    if (current)
    {
        if (!pressed)
        {
            pressed = true;

            pressedTime = millis();
        }

        if (millis() - pressedTime >= 10000)
        {
            _devices.ResetController();

            pressed = false;
        }
    }
    else
    {
        pressed = false;
    }
}