#include "Fingerprint.h"

#include <ArduinoJson.h>

namespace
{
    constexpr uint16_t START_CODE = 0xEF01;

    constexpr uint8_t PACKET_COMMAND = 0x01;
    constexpr uint8_t PACKET_DATA    = 0x02;
    constexpr uint8_t PACKET_ACK     = 0x07;
    constexpr uint8_t PACKET_ENDDATA = 0x08;

    constexpr uint32_t DEFAULT_ADDRESS = 0xFFFFFFFF;

    constexpr uint16_t CMD_GET_IMAGE   = 0x0001;
    constexpr uint16_t CMD_UPLOAD_IMAGE = 0x000A;   // Used in Part 2
}

Fingerprint::Fingerprint(
    HardwareSerial& serial,
    MqttManager& mqtt,
    ConfigurationManager& configuration)
    :
    _serial(serial),
    _mqtt(mqtt),
    _configuration(configuration)
{
}

bool Fingerprint::Initialize()
{
    _serial.begin(
        57600,
        SERIAL_8N1);

    while (_serial.available())
    {
        _serial.read();
    }

    _mode = Mode::Idle;

    return true;
}

void Fingerprint::Update()
{
    switch (_mode)
    {
        case Mode::Capture:
            CaptureFingerprint();
            break;

        case Mode::Verify:
            VerifyFingerprint();
            break;

        default:
            break;
    }
}

void Fingerprint::RequestCapture(int personId)
{
    _personId = personId;
    _mode = Mode::Capture;
}

void Fingerprint::RequestVerification()
{
    _mode = Mode::Verify;
}

uint16_t Fingerprint::CalculateChecksum(
    const uint8_t* data,
    uint16_t length)
{
    uint16_t checksum = 0;

    for(uint16_t i = 0; i < length; i++)
    {
        checksum += data[i];
    }

    return checksum;
}

bool Fingerprint::SendPacket(
    uint16_t command,
    const uint8_t* data,
    uint16_t length)
{
    uint16_t packetLength = length + 3;

    _serial.write(highByte(START_CODE));
    _serial.write(lowByte(START_CODE));

    _serial.write(
        (DEFAULT_ADDRESS >> 24) & 0xFF);

    _serial.write(
        (DEFAULT_ADDRESS >> 16) & 0xFF);

    _serial.write(
        (DEFAULT_ADDRESS >> 8) & 0xFF);

    _serial.write(
        DEFAULT_ADDRESS & 0xFF);

    _serial.write(PACKET_COMMAND);

    _serial.write(highByte(packetLength));
    _serial.write(lowByte(packetLength));

    _serial.write(highByte(command));
    _serial.write(lowByte(command));

    for(uint16_t i = 0; i < length; i++)
    {
        _serial.write(data[i]);
    }

    uint16_t checksum =
        PACKET_COMMAND +
        highByte(packetLength) +
        lowByte(packetLength) +
        highByte(command) +
        lowByte(command);

    for(uint16_t i = 0; i < length; i++)
    {
        checksum += data[i];
    }

    _serial.write(highByte(checksum));
    _serial.write(lowByte(checksum));

    return true;
}

bool Fingerprint::ReadPacket(
    uint8_t* buffer,
    uint32_t& length,
    uint32_t timeout)
{
    uint32_t start = millis();

    while(_serial.available() == 0)
    {
        if(millis() - start > timeout)
        {
            return false;
        }
    }

    length = 0;

    while(true)
    {
        while(_serial.available())
        {
            buffer[length++] = _serial.read();

            if(length >= 4096)
            {
                return true;
            }
        }

        if(millis() - start > timeout)
        {
            break;
        }
    }

    return length > 0;
}

bool Fingerprint::WaitForAck(
    uint16_t expectedCommand,
    uint32_t timeout)
{
    uint8_t buffer[32];

    uint32_t length = 0;

    if(!ReadPacket(
        buffer,
        length,
        timeout))
    {
        return false;
    }

    if(length < 12)
    {
        return false;
    }

    if(buffer[6] != PACKET_ACK)
    {
        return false;
    }

    uint16_t response =
        (buffer[9] << 8) |
         buffer[10];

    return response == expectedCommand;
}