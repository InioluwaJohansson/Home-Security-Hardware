#pragma once
#include <Arduino.h>
#include "IHardwareController.h"

class Multiplexer;

class Hardware : public IHardwareController
{
public:

    Hardware(
        Multiplexer& lights,
        Multiplexer& appliances,
        Multiplexer& doors,
        Multiplexer& windows,
        Multiplexer& cameras);

    void Initialize() override;

    void SetLight(int id,bool active) override;

    void SetLightBrightness(
        int id,
        uint8_t brightness) override;

    void SetAppliance(int id,bool active) override;

    void SetCamera(int id,bool active) override;

    void LockDoor(int id) override;

    void UnlockDoor(int id) override;

    void OpenDoor(int id) override;

    void CloseDoor(int id) override;

    void LockWindow(int id) override;

    void UnlockWindow(int id) override;

    void OpenWindow(int id) override;

    void CloseWindow(int id) override;

    bool ReadDoorClosed(int id) override;

    bool ReadWindowClosed(int id) override;

    bool ReadExternal(int id) override;

private:

    Multiplexer& _lights;
    Multiplexer& _appliances;
    Multiplexer& _doors;
    Multiplexer& _windows;
    Multiplexer& _cameras;
};