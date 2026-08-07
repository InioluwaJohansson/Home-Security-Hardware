#include "HardwareController.h"
#include "IHardwareController.h"
#include "Hardware.h"
#include "MCP23017.h"
#include "Multiplexer.h"
#include "PCA9685.h"
#include "Hardware.h"

Hardware::Hardware(
    HardwareManager& manager,
    MCP23017Controller& mcp,
    PCA9685Controller& pwm,
    MultiplexerController& multiplexer)
    :
    _manager(manager),
    _mcp(mcp),
    _pwm(pwm),
    _multiplexer(multiplexer)
{
}

bool Hardware::Initialize()
{
    return
        _mcp.Initialize() &&
        _pwm.Initialize();
}

//--------------------------------------------------
// Lights
//--------------------------------------------------

void Hardware::SetLight(
    int id,
    bool active)
{
    auto* light = _manager.GetLight(id);

    if(light == nullptr)
        return;

    _mcp.DigitalWrite(
        light->Primary.Chip,
        light->Primary.Pin,
        active);
}

void HardwareController::SetLightBrightness(
    int id,
    uint8_t brightness)
{
    auto* light = _manager.GetLight(id);

    if(light == nullptr)
        return;

    _pwm.SetBrightness(
        light->Pwm.PwmChannel,
        brightness);
}

//--------------------------------------------------
// Appliances
//--------------------------------------------------

void HardwareController::SetAppliance(
    int id,
    bool active)
{
    auto* appliance = _manager.GetAppliance(id);

    if(appliance == nullptr)
        return;

    _mcp.DigitalWrite(
        appliance->Primary.Chip,
        appliance->Primary.Pin,
        active);
}

//--------------------------------------------------
// Cameras
//--------------------------------------------------

void HardwareController::SetCamera(
    int id,
    bool active)
{
    auto* camera = _manager.GetCamera(id);

    if(camera == nullptr)
        return;

    _mcp.DigitalWrite(
        camera->Primary.Chip,
        camera->Primary.Pin,
        active);
}

//--------------------------------------------------
// Doors
//--------------------------------------------------

void HardwareController::LockDoor(int id)
{
    auto* door = _manager.GetDoor(id);

    if(door == nullptr)
        return;

    _mcp.DigitalWrite(
        door->Primary.Chip,
        door->Primary.Pin,
        true);
}

void HardwareController::UnlockDoor(int id)
{
    auto* door = _manager.GetDoor(id);

    if(door == nullptr)
        return;

    _mcp.DigitalWrite(
        door->Primary.Chip,
        door->Primary.Pin,
        false);
}

void HardwareController::OpenDoor(int id)
{
    auto* door = _manager.GetDoor(id);

    if(door == nullptr)
        return;

    _mcp.DigitalWrite(
        door->Motor.Chip,
        door->Motor.ForwardPin,
        true);

    _mcp.DigitalWrite(
        door->Motor.Chip,
        door->Motor.ReversePin,
        false);
}

void HardwareController::CloseDoor(int id)
{
    auto* door = _manager.GetDoor(id);

    if(door == nullptr)
        return;

    _mcp.DigitalWrite(
        door->Motor.Chip,
        door->Motor.ForwardPin,
        false);

    _mcp.DigitalWrite(
        door->Motor.Chip,
        door->Motor.ReversePin,
        true);
}

bool HardwareController::IsDoorClosed(int id)
{
    auto* door = _manager.GetDoor(id);

    if(door == nullptr)
        return false;

    return _multiplexer.DigitalRead(
        door->Sensor.MultiplexerChannel);
}

//--------------------------------------------------
// Windows
//--------------------------------------------------

void HardwareController::LockWindow(int id)
{
    auto* window = _manager.GetWindow(id);

    if(window == nullptr)
        return;

    _mcp.DigitalWrite(window->Primary.Chip,
                      window->Primary.Pin,
                      true);
}

void HardwareController::UnlockWindow(int id)
{
    auto* window = _manager.GetWindow(id);

    if(window == nullptr)
        return;

    _mcp.DigitalWrite(window->Primary.Chip,
                      window->Primary.Pin,
                      false);
}

void HardwareController::OpenWindow(int id)
{
    auto* window = _manager.GetWindow(id);

    if(window == nullptr)
        return;

    _mcp.DigitalWrite(window->Motor.Chip,
                      window->Motor.ForwardPin,
                      true);

    _mcp.DigitalWrite(window->Motor.Chip,
                      window->Motor.ReversePin,
                      false);
}

void HardwareController::CloseWindow(int id)
{
    auto* window = _manager.GetWindow(id);

    if(window == nullptr)
        return;

    _mcp.DigitalWrite(window->Motor.Chip,
                      window->Motor.ForwardPin,
                      false);

    _mcp.DigitalWrite(window->Motor.Chip,
                      window->Motor.ReversePin,
                      true);
}

bool HardwareController::IsWindowClosed(int id)
{
    auto* window = _manager.GetWindow(id);

    if(window == nullptr)
        return false;

    return _multiplexer.DigitalRead(
        window->Sensor.MultiplexerChannel);
}

//--------------------------------------------------
// External
//--------------------------------------------------

void HardwareController::SetExternal(
    int id,
    bool active)
{
    auto* external = _manager.GetExternal(id);

    if(external == nullptr)
        return;

    _mcp.DigitalWrite(
        external->Primary.Chip,
        external->Primary.Pin,
        active);
}

bool HardwareController::ReadExternal(int id)
{
    auto* external = _manager.GetExternal(id);

    if(external == nullptr)
        return false;

    return _multiplexer.DigitalRead(
        external->Sensor.MultiplexerChannel);
}

//--------------------------------------------------
// Alarm
//--------------------------------------------------

void HardwareController::Alarm(bool active)
{
    _mcp.DigitalWrite(
        2,      // MCP #3 (zero-based index)
        1,      // Pin 1
        active);
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