#pragma once

#include <Arduino.h>

#include "../Hardware/IHardwareController.h"
#include "../Facilities/Devices.h"
#include "../Facilities/StateCache.h"
#include "../MQTT/MQTT.h"

class SensorController
{
public:

    SensorController(
        IHardwareController& hardware,
        DeviceController& devices,
        StateCache& cache,
        MqttManager& mqtt);

    void Initialize();

    void Update();

private:

    IHardwareController& _hardware;

    DeviceManager& _devices;

    StateCache& _cache;

    MqttManager& _mqtt;

    void UpdateDoors();

    void UpdateWindows();

    void UpdateExternals();

    void UpdateResetButton();
};