#pragma once

#include <Arduino.h>

#include "../Hardware/IHardwareController.h"
#include "../Facilities/Devices.h"
#include "../Facilities/StateCache.h"
#include "../Config/Config.h"
#include "../MQTT/MQTT.h"

class SensorController
{
public:
    SensorController(
        IHardwareController& hardware,
        ConfigurationManager& configuration,
        DeviceManager& devices,
        StateCache& cache,
        MqttManager& mqtt);

    void Initialize();
    void Update();

private:
    IHardwareController& _hardware;
    ConfigurationManager& _configuration;
    DeviceManager& _devices;
    StateCache& _cache;
    MqttManager& _mqtt;

    void UpdateDoors();
    void UpdateWindows();
    void UpdateExternals();
    void UpdateResetButton();
};
