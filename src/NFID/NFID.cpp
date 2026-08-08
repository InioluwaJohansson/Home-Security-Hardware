#include "NFID.h"

NFID::NFID(
    ConfigurationManager& configuration,
    MqttManager& mqtt,
    TwoWire& wire)
    :
    _configuration(configuration),
    _mqtt(mqtt),
    _protocol(wire)
{
}

NFID::NFID(
    ConfigurationManager& configuration,
    MqttManager& mqtt,
    uint8_t,
    uint8_t)
    :
    _configuration(configuration),
    _mqtt(mqtt),
    _protocol(Wire)
{
}

bool NFID::Initialize()
{
    _context = NFIDContext{};
    return _protocol.Initialize();
}

void NFID::Update()
{
    switch (_context.Operation)
    {
        case NFIDOperation::Write:
            HandleWrite();
            break;

        case NFIDOperation::Read:
        case NFIDOperation::None:
        default:
            HandleRead();
            break;
    }
}

void NFID::QueueWrite(const String& nfidCode)
{
    if (nfidCode.isEmpty())
    {
        _context.State = NFIDState::Failed;
        return;
    }

    _context.Operation = NFIDOperation::Write;
    _context.State = NFIDState::WaitingForCard;
    _context.NFIDCode = nfidCode;
    _context.StartedAt = millis();
}

void NFID::HandleRead()
{
    static unsigned long lastReadTime = 0;

    if (millis() - lastReadTime < 1500)
        return;

    if (!_protocol.CardPresent())
        return;

    _context.Operation = NFIDOperation::Read;
    _context.State = NFIDState::Reading;

    String code;

    if (!_protocol.ReadCode(code))
    {
        // Non-Home cards are ignored/rejected.
        _context.State = NFIDState::Failed;
        _context.Operation = NFIDOperation::None;
        lastReadTime = millis();
        return;
    }

    _context.NFIDCode = code;
    _context.State = NFIDState::Publishing;

    PublishVerification(code);

    lastReadTime = millis();

    _context.Operation = NFIDOperation::None;
    _context.State = NFIDState::Success;
    _context.NFIDCode.clear();
}

void NFID::HandleWrite()
{
    static constexpr unsigned long WriteTimeout = 30000;

    if (millis() - _context.StartedAt > WriteTimeout)
    {
        _context.State = NFIDState::Failed;
        _context.Operation = NFIDOperation::None;
        _context.NFIDCode.clear();
        _context.StartedAt = 0;
        return;
    }

    if (!_protocol.CardPresent())
        return;

    _context.State = NFIDState::Writing;

    const bool success =
        _protocol.WriteCode(_context.NFIDCode);

    _context.State =
        success ? NFIDState::Success : NFIDState::Failed;

    _context.Operation = NFIDOperation::None;
    _context.NFIDCode.clear();
    _context.StartedAt = 0;
}

void NFID::PublishVerification(const String& code)
{
    NFIDVerifyDto dto;

    dto.Token = _configuration.GetJwt();
    dto.HardwareId =
        _configuration.GetConfiguration().HardwareId;
    dto.NFIDCode = code;

    _mqtt.PublishNFIDVerify(dto);
}
