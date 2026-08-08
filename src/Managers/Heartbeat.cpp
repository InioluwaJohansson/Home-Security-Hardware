#include "Heartbeat.h"

#include "../Config/Config.h"
#include "../MQTT/MQTT.h"

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
    const unsigned long now = millis();

    const unsigned long interval =
        static_cast<unsigned long>(
            _configuration.GetConfiguration().HeartbeatInterval) *
        1000UL;

    if (interval > 0 &&
        now - _lastHeartbeat < interval)
    {
        return;
    }

    _lastHeartbeat = now;

    const auto& configuration =
        _configuration.GetConfiguration();

    HardwareHeartBeatDto dto;

    dto.Token = _configuration.GetJwt();
    dto.HardwareId = configuration.HardwareId;
    dto.PowerActive = true;

    _mqtt.PublishHeartbeat(dto);
}
