#include "NFID.h"

#include "../Config/Config.h"
#include "../MQTT/MqttManager.h"
#include "NFIDModels.h"

NFID::NFID(
    ConfigurationManager& configuration,
    MqttManager& mqtt,
    uint8_t sda,
    uint8_t scl)
    :
    _configuration(configuration),
    _mqtt(mqtt),
    _protocol(sda, scl)
{
}

bool NFID::Initialize()
{
    _context.Operation = NFIDOperation::None;

    _context.State = NFIDState::Idle;

    _context.NFIDCode.clear();

    _context.StartedAt = 0;

    return _protocol.Initialize();
}

void NFID::Update()
{
    switch (_context.Operation)
    {
        case NFIDOperation::Write:
        {
            HandleWrite();
            break;
        }

        case NFIDOperation::Read:
        case NFIDOperation::None:
        default:
        {
            HandleRead();
            break;
        }
    }
}

void NFID::QueueWrite(
    const String& nfidCode)
{
    _context.Operation = NFIDOperation::Write;

    _context.State = NFIDState::WaitingForCard;

    _context.NFIDCode = nfidCode;

    _context.StartedAt = millis();
}

void NFID::HandleRead()
{
    static unsigned long lastReadTime = 0;

    if (millis() - lastReadTime < 1500)
    {
        return;
    }

    if (!_protocol.CardPresent())
    {
        return;
    }

    _context.Operation = NFIDOperation::Read;

    _context.State = NFIDState::Reading;

    String nfidCode;

    if (!_protocol.ReadCode(nfidCode))
    {
        _context.State = NFIDState::Failed;

        _context.Operation = NFIDOperation::None;

        return;
    }

    _context.NFIDCode = nfidCode;

    _context.State = NFIDState::Publishing;

    PublishVerification(nfidCode);

    lastReadTime = millis();

    _context.Operation = NFIDOperation::None;

    _context.State = NFIDState::Success;

    _context.NFIDCode.clear();
}
void NFID::HandleWrite()
{
    static constexpr unsigned long WriteTimeout = 30000;

    if ((millis() - _context.StartedAt) > WriteTimeout)
    {
        _context.State = NFIDState::Failed;

        _context.Operation = NFIDOperation::None;

        _context.NFIDCode.clear();

        return;
    }

    if (!_protocol.CardPresent())
    {
        return;
    }

    _context.State = NFIDState::Writing;

    if (_protocol.WriteCode(_context.NFIDCode))
    {
        _context.State = NFIDState::Success;
    }
    else
    {
        _context.State = NFIDState::Failed;
    }

    _context.Operation = NFIDOperation::None;

    _context.NFIDCode.clear();

    _context.StartedAt = 0;
}

void NFID::PublishVerification(
    const String& nfidCode)
{
    NFIDVerifyDto dto;

    dto.Token = _configuration.GetJwt();

    dto.HardwareId =
        _configuration
            .GetConfiguration()
            .HardwareId;

    dto.NFIDCode = nfidCode;

    _mqtt.PublishNFIDVerify(dto);
}