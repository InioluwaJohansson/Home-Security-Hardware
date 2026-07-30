#include "Heartbeat.h"

#include "../Configuration/ConfigurationManager.h"
#include "../MQTT/MqttManager.h"
#include "../Models/HardwareHeartBeatDto.h"

Heartbeat::Heartbeat(
    ConfigurationManager& configuration,
    MqttManager& mqtt)
    :
    _configuration(configuration),
    _mqtt(mqtt),
    _lastHeartbeat(0)
{
}

void Heartbeat::Update()
{
    const unsigned long currentTime = millis();

    if (currentTime - _lastHeartbeat < Interval)
    {
        return;
    }

    _lastHeartbeat = currentTime;

    const auto& configuration = _configuration.GetConfiguration();

    HardwareHeartBeatDto dto;

    dto.Token = _configuration.GetJwt();
    dto.HardwareId = configuration.HardwareId;
    dto.PowerActive = true;

    _mqtt.PublishHeartbeat(dto);
}
