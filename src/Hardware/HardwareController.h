#pragma once

#include <Arduino.h>
#include "IHardwareController.h"
#include "Hardware.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "Multiplexer.h"
#include "ADS1115.h"

class HardwareController : public IHardwareController
{
public:
    HardwareController(
        HardwareManager& manager,
        MCP23017Controller& mcp,
        PCA9685Controller& pwm,
        MultiplexerController& multiplexer,
        ADS1115Controller& adc);

    void Initialize() override;
    void Update();

    void SetLight(int id, bool active) override;
    void SetBrightness(int id, int brightness) override;
    void SetAppliance(int id, bool active) override;
    void SetExternal(int id, bool active) override;
    void SetCamera(int id, bool active) override;

    void LockDoor(int id) override;
    void UnlockDoor(int id) override;
    void OpenDoor(int id) override;
    void CloseDoor(int id) override;

    void LockWindow(int id) override;
    void UnlockWindow(int id) override;
    void OpenWindow(int id) override;
    void CloseWindow(int id) override;

    bool ReadDoorClosed(int id) override;
    bool ReadDoorClosedSensor(int id) override;
    bool ReadDoorLockedSensor(int id) override;
    bool ReadWindowClosed(int id) override;
    bool ReadWindowClosedSensor(int id) override;
    bool ReadWindowLockedSensor(int id) override;
    bool ReadExternal(int id) override;
    bool ReadExternalSensor(int id) override;
    bool ReadResetButton() override;

    float ReadDoorMotorCurrent(int id) override;
    float ReadDoorLockCurrent(int id) override;
    float ReadWindowMotorCurrent(int id) override;
    float ReadWindowLockCurrent(int id) override;

    void SetBuzzer(bool active) override;
    void SetSiren(bool active) override;
    void Beep(uint16_t duration = 100) override;

private:
    struct MotionRuntime
    {
        bool active = false;
        bool opening = false;
        uint32_t startedAt = 0;
        uint8_t currentSpeed = 0;
    };

    struct LockRuntime
    {
        bool active = false;
        bool locking = false;
        uint32_t startedAt = 0;
    };

    HardwareManager& _manager;
    MCP23017Controller& _mcp;
    PCA9685Controller& _pwm;
    MultiplexerController& _multiplexer;
    ADS1115Controller& _adc;

    MotionRuntime _doorMotion[MAX_DOORS];
    MotionRuntime _windowMotion[MAX_WINDOWS];
    LockRuntime _doorLocks[MAX_DOORS];
    LockRuntime _windowLocks[MAX_WINDOWS];

    void SetMotor(
        MotorizedFacilityHardware& resource,
        bool forward,
        uint8_t speed);

    void StopMotor(
        MotorizedFacilityHardware& resource);

    void SetLock(
        MotorizedFacilityHardware& resource,
        bool locked);

    void UpdateMotor(
        MotorizedFacilityHardware& resource,
        MotionRuntime& runtime);

    void UpdateLock(
        MotorizedFacilityHardware& resource,
        LockRuntime& runtime);

    bool ReadSensor(const SensorEndpoint& sensor);
    float ReadCurrent(const CurrentSensorEndpoint& sensor);
};
