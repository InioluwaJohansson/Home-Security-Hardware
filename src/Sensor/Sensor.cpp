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
    UpdateAlarm();
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

        GenericDeviceUpdateState update;

        update.Type = FacilityType::Door;
        update.Id = state.DoorId;
        update.IsOpen = state.IsOpen;
        update.IsLocked = state.IsLocked;

        _devices.NotifyPhysicalStateChanged(update);
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

        GenericDeviceUpdateState update;

        update.Type = FacilityType::Window;
        update.Id = state.WindowId;
        update.IsOpen = state.IsOpen;
        update.IsLocked = state.IsLocked;

        _devices.NotifyPhysicalStateChanged(update);
    }
}

void SensorController::UpdateExternals()
{
    for (int i = 0; i < MAX_EXTERNALS; ++i)
    {
        // IsActive is the enable/configuration flag.
        // If the external input is disabled, do NOT read the PIR.
        if (!_cache.HasExternal(i))
            continue;

        ExternalDeviceState state = _cache.GetExternal(i);

        if (!state.IsActive)
            continue;

        const bool detected =
            _hardware.ReadExternalSensor(i);

        // A physical PIR detection latches the alarm trigger. The ESP32
        // must NOT clear IsTriggered merely because the PIR returns LOW.
        // The backend owns the reset by sending IsTriggered=false.
        if (!detected || state.IsTriggered)
            continue;

        state.IsTriggered = true;

        _cache.UpdateExternal(state);

        GenericDeviceUpdateState update;

        update.Type = FacilityType::External;
        update.Id = state.ExternalId;
        update.IsActive = state.IsActive;
        update.IsTriggered = state.IsTriggered;

        _devices.NotifyPhysicalStateChanged(update);
    }
}

void SensorController::UpdateAlarm()
{
    static bool lastAlarmTriggered = false;

    bool alarmTriggered = false;

    for (int i = 0; i < MAX_EXTERNALS; ++i)
    {
        if (!_cache.HasExternal(i))
            continue;

        const ExternalDeviceState state = _cache.GetExternal(i);

        if (state.IsActive && state.IsTriggered)
        {
            alarmTriggered = true;
            break;
        }
    }

    // Alarm state is persistent until the backend clears every triggered
    // external. There is deliberately no timeout here.
    if (alarmTriggered == lastAlarmTriggered)
        return;

    _hardware.SetBuzzer(alarmTriggered);
    _hardware.SetSiren(alarmTriggered);
    lastAlarmTriggered = alarmTriggered;
}

void SensorController::UpdateResetButton()
{
    static bool rawState = false;
    static bool stableState = false;
    static unsigned long rawChangedAt = 0;
    static unsigned long pressedAt = 0;
    static bool resetHandled = false;

    const unsigned long now = millis();
    const bool raw = _hardware.ReadResetButton();

    if (raw != rawState)
    {
        rawState = raw;
        rawChangedAt = now;
    }

    if (now - rawChangedAt < 50)
        return;

    if (stableState != rawState)
    {
        stableState = rawState;

        if (stableState)
        {
            pressedAt = now;
            resetHandled = false;
        }
        else
        {
            resetHandled = false;
        }
    }

    if (stableState &&
        !resetHandled &&
        now - pressedAt >= 10000)
    {
        resetHandled = true;

        Serial.println("[RESET] Preferences reset requested");

        _hardware.SetBuzzer(true);
        _hardware.SetSiren(true);
        delay(250);

        if (_configuration.ResetPreferences())
        {
            Serial.println("[RESET] Preferences cleared; restarting");
            _hardware.SetBuzzer(false);
            _hardware.SetSiren(false);
            delay(100);
            ESP.restart();
        }

        _hardware.SetBuzzer(false);
        _hardware.SetSiren(false);
    }
}
