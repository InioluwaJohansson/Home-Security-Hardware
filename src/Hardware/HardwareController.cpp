#include "HardwareController.h"
#include "Pins.h"
#include <cmath>

HardwareController::HardwareController(
    HardwareManager& manager,
    MCP23017Controller& mcp,
    PCA9685Controller& pwm,
    MultiplexerController& multiplexer,
    ADS1115Controller& adc)
    :
    _manager(manager),
    _mcp(mcp),
    _pwm(pwm),
    _multiplexer(multiplexer),
    _adc(adc)
{
}

void HardwareController::Initialize()
{
    Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);
    Wire.setClock(400000);

    _multiplexer.Initialize();

    if (!_mcp.Initialize())
        Serial.println("[HARDWARE] MCP23017 initialization failed");

    if (!_pwm.Initialize())
        Serial.println("[HARDWARE] PCA9685 initialization failed");

    if (!_adc.Initialize())
        Serial.println("[HARDWARE] ADS1115 initialization failed");

    pinMode(Pins::RESET_BUTTON, INPUT_PULLUP);

    // Explicitly force every motor PWM channel off before anything can move.
    for (uint8_t i = 0; i < 16; ++i)
        _pwm.TurnOff(0, i);
}

void HardwareController::Update()
{
    for (uint8_t i = 0; i < MAX_DOORS; ++i)
    {
        if (auto* door = _manager.GetDoorSlot(i))
        {
            UpdateMotor(door->motorized, _doorMotion[i]);
            UpdateLock(door->motorized, _doorLocks[i]);
        }
    }

    for (uint8_t i = 0; i < MAX_WINDOWS; ++i)
    {
        if (auto* window = _manager.GetWindowSlot(i))
        {
            UpdateMotor(window->motorized, _windowMotion[i]);
            UpdateLock(window->motorized, _windowLocks[i]);
        }
    }
}

bool HardwareController::ReadSensor(const SensorEndpoint& sensor)
{
    if (sensor.bus == SensorBus::None || sensor.channel == INVALID_CHANNEL)
        return false;

    switch (sensor.bus)
    {
        case SensorBus::DoorMux:
            return _multiplexer.DigitalRead(
                MultiplexerController::Bus::Door,
                sensor.channel,
                sensor.activeHigh);

        case SensorBus::WindowMux:
            return _multiplexer.DigitalRead(
                MultiplexerController::Bus::Window,
                sensor.channel,
                sensor.activeHigh);

        case SensorBus::ExternalMux:
            return _multiplexer.DigitalRead(
                MultiplexerController::Bus::External,
                sensor.channel,
                sensor.activeHigh);

        case SensorBus::McuGpio:
            return _mcp.DigitalRead(
                MCP23017Controller::CHIP_COUNT - 1,
                sensor.channel) == (sensor.activeHigh ? HIGH : LOW);

        default:
            return false;
    }
}

float HardwareController::ReadCurrent(const CurrentSensorEndpoint& sensor)
{
    if (sensor.adc == INVALID_CHANNEL || sensor.channel == INVALID_CHANNEL)
        return 0.0f;

    return _adc.ReadCurrent(
        sensor.adc,
        sensor.channel,
        sensor.zeroVoltage,
        sensor.voltsPerAmp);
}

void HardwareController::SetLight(int id, bool active)
{
    auto* light = _manager.GetLight(id);
    if (!light) return;

    _mcp.DigitalWrite(light->primary.chip, light->primary.pin, active);

    if (!active)
        _pwm.TurnOff(light->pwm.controller, light->pwm.channel);
}

void HardwareController::SetBrightness(int id, int brightness)
{
    auto* light = _manager.GetLight(id);
    if (!light) return;

    if (brightness < 0) brightness = 0;
    if (brightness > 100) brightness = 100;

    _pwm.SetPercent(
        light->pwm.controller,
        light->pwm.channel,
        static_cast<uint8_t>(brightness));

    _mcp.DigitalWrite(
        light->primary.chip,
        light->primary.pin,
        brightness > 0);
}

void HardwareController::SetAppliance(int id, bool active)
{
    auto* appliance = _manager.GetAppliance(id);
    if (!appliance) return;
    _mcp.DigitalWrite(appliance->primary.chip, appliance->primary.pin, active);
}

void HardwareController::SetDoorType(int id, DoorType type)
{
    auto* door = _manager.GetDoor(id);
    if (!door)
        return;

    door->doorType = type;

    switch (type)
    {
        case DoorType::Exterior:
            door->motorized.motor.maxSpeed = 100;
            door->motorized.motor.acceleration = 30;
            door->motorized.motor.deceleration = 30;
            break;

        case DoorType::Interior:
        case DoorType::Gate:
        default:
            door->motorized.motor.maxSpeed = 65;
            door->motorized.motor.acceleration = 25;
            door->motorized.motor.deceleration = 25;
            break;
    }
}

void HardwareController::StartLock(
    MotorizedFacilityHardware& resource,
    LockRuntime& runtime,
    bool locking)
{
    runtime.active = true;
    runtime.locking = locking;
    runtime.pendingAfterClose = false;
    runtime.startedAt = millis();

    SetLock(resource, locking);
}

void HardwareController::SetCamera(int id, bool active)
{
    auto* camera = _manager.GetCamera(id);
    if (!camera) return;
    _mcp.DigitalWrite(camera->primary.chip, camera->primary.pin, active);
}

void HardwareController::SetExternal(int id, bool active)
{
    auto* external = _manager.GetExternal(id);
    if (!external) return;
    _mcp.DigitalWrite(external->primary.chip, external->primary.pin, active);
}

void HardwareController::SetMotor(
    MotorizedFacilityHardware& resource,
    bool forward,
    uint8_t speed)
{
    _mcp.DigitalWrite(
        resource.motor.direction.chip,
        resource.motor.direction.pin,
        forward ? resource.motor.openDirection : !resource.motor.openDirection);

    _pwm.SetPercent(
        resource.motor.pwm.controller,
        resource.motor.pwm.channel,
        speed);
}

void HardwareController::StopMotor(MotorizedFacilityHardware& resource)
{
    _pwm.TurnOff(
        resource.motor.pwm.controller,
        resource.motor.pwm.channel);
}

void HardwareController::SetLock(
    MotorizedFacilityHardware& resource,
    bool locked)
{
    _mcp.DigitalWrite(
        resource.lock.direction.chip,
        resource.lock.direction.pin,
        locked ? resource.lock.lockDirection : !resource.lock.lockDirection);

    _pwm.SetPercent(
        resource.lock.pwm.controller,
        resource.lock.pwm.channel,
        100);
}

void HardwareController::UpdateMotor(
    MotorizedFacilityHardware& resource,
    MotionRuntime& runtime)
{
    if (!runtime.active)
        return;

    const uint32_t elapsed = millis() - runtime.startedAt;
    const bool targetReached = runtime.opening
        ? ReadSensor(resource.motor.openSensor)
        : ReadSensor(resource.motor.closeSensor);
    const float current = std::fabs(ReadCurrent(resource.motor.current));

    if (targetReached ||
        current > static_cast<float>(resource.motor.maxCurrent) / 1000.0f ||
        elapsed >= resource.motor.maxTravelTime)
    {
        StopMotor(resource);
        runtime.active = false;
        runtime.currentSpeed = 0;

        // Only a confirmed closed Hall sensor may transition a queued
        // close-then-lock request into lock motion. Current-limit trips and
        // timeouts deliberately leave the lock untouched.
        if (targetReached && !runtime.opening)
        {
            for (uint8_t i = 0; i < MAX_DOORS; ++i)
            {
                auto* door = _manager.GetDoorSlot(i);
                if (door && &resource == &door->motorized &&
                    _doorLocks[i].pendingAfterClose)
                {
                    StartLock(resource, _doorLocks[i], true);
                    return;
                }
            }

            for (uint8_t i = 0; i < MAX_WINDOWS; ++i)
            {
                auto* window = _manager.GetWindowSlot(i);
                if (window && &resource == &window->motorized &&
                    _windowLocks[i].pendingAfterClose)
                {
                    StartLock(resource, _windowLocks[i], true);
                    return;
                }
            }
        }

        return;
    }

    // Accelerate until the configured maximum. Begin a simple non-blocking
    // deceleration near the configured travel-time limit so the mechanism is
    // not commanded at full speed all the way to a Hall end-stop.
    const uint16_t decelStart = resource.motor.maxTravelTime > 2000
        ? resource.motor.maxTravelTime - 1500
        : resource.motor.maxTravelTime / 2;

    if (elapsed >= decelStart)
    {
        runtime.decelerating = true;
        const uint8_t step = resource.motor.deceleration / 10 > 0
            ? resource.motor.deceleration / 10
            : 1;

        runtime.currentSpeed = runtime.currentSpeed > step
            ? runtime.currentSpeed - step
            : 1;
    }
    else if (runtime.currentSpeed < resource.motor.maxSpeed)
    {
        const uint8_t step = resource.motor.acceleration / 10 > 0
            ? resource.motor.acceleration / 10
            : 1;

        const uint16_t nextSpeed =
            static_cast<uint16_t>(runtime.currentSpeed) + step;

        runtime.currentSpeed =
            nextSpeed > resource.motor.maxSpeed
                ? resource.motor.maxSpeed
                : static_cast<uint8_t>(nextSpeed);
    }

    SetMotor(resource, runtime.opening, runtime.currentSpeed);
}

void HardwareController::UpdateLock(
    MotorizedFacilityHardware& resource,
    LockRuntime& runtime)
{
    if (!runtime.active)
        return;

    const uint32_t elapsed = millis() - runtime.startedAt;
    const bool targetReached =
        ReadSensor(resource.lock.positionSensor) == runtime.locking;
    const float current = std::fabs(ReadCurrent(resource.lock.current));

    if (targetReached ||
        current > static_cast<float>(resource.lock.maxCurrent) / 1000.0f ||
        elapsed >= resource.lock.maxTravelTime)
    {
        _pwm.TurnOff(
            resource.lock.pwm.controller,
            resource.lock.pwm.channel);

        runtime.active = false;
        runtime.pendingAfterClose = false;
    }
}

void HardwareController::LockDoor(int id)
{
    auto* door = _manager.GetDoor(id);
    if (!door || id < 0 || id >= MAX_DOORS) return;

    if (!ReadDoorClosed(id))
    {
        _doorLocks[id].pendingAfterClose = true;
        CloseDoor(id);
        return;
    }

    StartLock(door->motorized, _doorLocks[id], true);
}

void HardwareController::UnlockDoor(int id)
{
    auto* door = _manager.GetDoor(id);
    if (!door || id < 0 || id >= MAX_DOORS) return;

    _doorLocks[id].pendingAfterClose = false;
    StartLock(door->motorized, _doorLocks[id], false);
}

void HardwareController::OpenDoor(int id)
{
    auto* door = _manager.GetDoor(id);
    if (!door || id < 0 || id >= MAX_DOORS) return;

    if (ReadDoorLockedSensor(id)) return;

    _doorLocks[id].pendingAfterClose = false;
    StopMotor(door->motorized);
    SetMotor(door->motorized, true, 1);
    _doorMotion[id].active = true;
    _doorMotion[id].opening = true;
    _doorMotion[id].startedAt = millis();
    _doorMotion[id].currentSpeed = 1;
    _doorMotion[id].decelerating = false;
}

void HardwareController::CloseDoor(int id)
{
    auto* door = _manager.GetDoor(id);
    if (!door || id < 0 || id >= MAX_DOORS) return;

    StopMotor(door->motorized);
    SetMotor(door->motorized, false, 1);
    _doorMotion[id].active = true;
    _doorMotion[id].opening = false;
    _doorMotion[id].startedAt = millis();
    _doorMotion[id].currentSpeed = 1;
    _doorMotion[id].decelerating = false;
}

void HardwareController::LockWindow(int id)
{
    auto* window = _manager.GetWindow(id);
    if (!window || id < 0 || id >= MAX_WINDOWS) return;

    if (!ReadWindowClosed(id))
    {
        _windowLocks[id].pendingAfterClose = true;
        CloseWindow(id);
        return;
    }

    StartLock(window->motorized, _windowLocks[id], true);
}

void HardwareController::UnlockWindow(int id)
{
    auto* window = _manager.GetWindow(id);
    if (!window || id < 0 || id >= MAX_WINDOWS) return;

    _windowLocks[id].pendingAfterClose = false;
    StartLock(window->motorized, _windowLocks[id], false);
}

void HardwareController::OpenWindow(int id)
{
    auto* window = _manager.GetWindow(id);
    if (!window || id < 0 || id >= MAX_WINDOWS) return;

    if (ReadWindowLockedSensor(id)) return;

    _windowLocks[id].pendingAfterClose = false;
    StopMotor(window->motorized);
    SetMotor(window->motorized, true, 1);
    _windowMotion[id].active = true;
    _windowMotion[id].opening = true;
    _windowMotion[id].startedAt = millis();
    _windowMotion[id].currentSpeed = 1;
    _windowMotion[id].decelerating = false;
}

void HardwareController::CloseWindow(int id)
{
    auto* window = _manager.GetWindow(id);
    if (!window || id < 0 || id >= MAX_WINDOWS) return;

    StopMotor(window->motorized);
    SetMotor(window->motorized, false, 1);
    _windowMotion[id].active = true;
    _windowMotion[id].opening = false;
    _windowMotion[id].startedAt = millis();
    _windowMotion[id].currentSpeed = 1;
    _windowMotion[id].decelerating = false;
}

bool HardwareController::ReadDoorClosed(int id)
{
    auto* door = _manager.GetDoor(id);
    return door ? ReadSensor(door->motorized.motor.closeSensor) : false;
}

bool HardwareController::ReadDoorClosedSensor(int id)
{
    return ReadDoorClosed(id);
}

bool HardwareController::ReadDoorLockedSensor(int id)
{
    auto* door = _manager.GetDoor(id);
    return door ? ReadSensor(door->motorized.lock.positionSensor) : false;
}

bool HardwareController::ReadWindowClosed(int id)
{
    auto* window = _manager.GetWindow(id);
    return window ? ReadSensor(window->motorized.motor.closeSensor) : false;
}

bool HardwareController::ReadWindowClosedSensor(int id)
{
    return ReadWindowClosed(id);
}

bool HardwareController::ReadWindowLockedSensor(int id)
{
    auto* window = _manager.GetWindow(id);
    return window ? ReadSensor(window->motorized.lock.positionSensor) : false;
}

bool HardwareController::ReadExternal(int id)
{
    auto* external = _manager.GetExternal(id);
    return external ? ReadSensor(external->sensor) : false;
}

bool HardwareController::ReadExternalSensor(int id)
{
    return ReadExternal(id);
}

bool HardwareController::ReadResetButton()
{
    return digitalRead(Pins::RESET_BUTTON) == LOW;
}

float HardwareController::ReadDoorMotorCurrent(int id)
{
    auto* door = _manager.GetDoor(id);
    return door ? ReadCurrent(door->motorized.motor.current) : 0.0f;
}

float HardwareController::ReadDoorLockCurrent(int id)
{
    auto* door = _manager.GetDoor(id);
    return door ? ReadCurrent(door->motorized.lock.current) : 0.0f;
}

float HardwareController::ReadWindowMotorCurrent(int id)
{
    auto* window = _manager.GetWindow(id);
    return window ? ReadCurrent(window->motorized.motor.current) : 0.0f;
}

float HardwareController::ReadWindowLockCurrent(int id)
{
    auto* window = _manager.GetWindow(id);
    return window ? ReadCurrent(window->motorized.lock.current) : 0.0f;
}

void HardwareController::SetBuzzer(bool active)
{
    _mcp.DigitalWrite(
        HardwareOutputs::AlarmChip,
        HardwareOutputs::BuzzerPin,
        active);
}

void HardwareController::SetSiren(bool active)
{
    _mcp.DigitalWrite(
        HardwareOutputs::AlarmChip,
        HardwareOutputs::SirenPin,
        active);
}

void HardwareController::Beep(uint16_t duration)
{
    SetBuzzer(true);
    delay(duration);
    SetBuzzer(false);
}
