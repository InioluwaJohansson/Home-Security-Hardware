#pragma once

#include <Arduino.h>

class IHardwareController
{
public:
    virtual ~IHardwareController() = default;

    virtual void Initialize() = 0;

    virtual void SetLight(int id, bool active) = 0;
    virtual void SetBrightness(int id, int brightness) = 0;
    virtual void SetAppliance(int id, bool active) = 0;
    virtual void SetExternal(int id, bool active) = 0;
    virtual void SetCamera(int id, bool active) = 0;

    virtual void LockDoor(int id) = 0;
    virtual void UnlockDoor(int id) = 0;
    virtual void OpenDoor(int id) = 0;
    virtual void CloseDoor(int id) = 0;

    virtual void LockWindow(int id) = 0;
    virtual void UnlockWindow(int id) = 0;
    virtual void OpenWindow(int id) = 0;
    virtual void CloseWindow(int id) = 0;

    virtual bool ReadDoorClosed(int id) = 0;
    virtual bool ReadDoorClosedSensor(int id) = 0;
    virtual bool ReadDoorLockedSensor(int id) = 0;

    virtual bool ReadWindowClosed(int id) = 0;
    virtual bool ReadWindowClosedSensor(int id) = 0;
    virtual bool ReadWindowLockedSensor(int id) = 0;

    virtual bool ReadExternal(int id) = 0;
    virtual bool ReadExternalSensor(int id) = 0;
    virtual bool ReadResetButton() = 0;

    virtual float ReadDoorMotorCurrent(int id) = 0;
    virtual float ReadDoorLockCurrent(int id) = 0;
    virtual float ReadWindowMotorCurrent(int id) = 0;
    virtual float ReadWindowLockCurrent(int id) = 0;

    virtual void SetBuzzer(bool active) = 0;
    virtual void SetSiren(bool active) = 0;
    virtual void Beep(uint16_t duration = 100) = 0;
};
