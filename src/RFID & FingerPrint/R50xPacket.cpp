#include "R50xPacket.h"

R30xPacket::R30xPacket()
{
    Clear();
}

void R30xPacket::Clear()
{
    _address = R30x::Address;

    _type = R30x::PacketType::CommandPacket;

    _payloadLength = 0;

    _checksum = 0;

    _valid = false;

    memset(
        _payload,
        0,
        sizeof(_payload));
}

void R30xPacket::SetAddress(
    uint32_t address)
{
    _address = address;
}

void R30xPacket::SetPacketType(
    R30x::PacketType type)
{
    _type = type;
}

void R30xPacket::SetInstruction(
    R30x::Instruction instruction)
{
    _payload[0] = static_cast<uint8_t>(instruction);

    _payloadLength = 1;
}

void R30xPacket::SetPayload(
    const uint8_t* data,
    uint16_t length)
{
    if(length > sizeof(_payload))
    {
        length = sizeof(_payload);
    }

    memcpy(
        _payload,
        data,
        length);

    _payloadLength = length;
}
uint16_t R30xPacket::CalculateChecksum() const
{
    uint16_t checksum = 0;

    checksum += static_cast<uint8_t>(_type);

    uint16_t length = _payloadLength + 2;

    checksum += highByte(length);
    checksum += lowByte(length);

    for(uint16_t i = 0; i < _payloadLength; i++)
    {
        checksum += _payload[i];
    }

    return checksum;
}
bool R30xPacket::Send(
    HardwareSerial& serial)
{
    _checksum = CalculateChecksum();

    serial.write(highByte(R30x::Header));
    serial.write(lowByte(R30x::Header));

    serial.write((_address >> 24) & 0xFF);
    serial.write((_address >> 16) & 0xFF);
    serial.write((_address >> 8) & 0xFF);
    serial.write(_address & 0xFF);

    serial.write(static_cast<uint8_t>(_type));

    uint16_t length = _payloadLength + 2;

    serial.write(highByte(length));
    serial.write(lowByte(length));

    serial.write(
        _payload,
        _payloadLength);

    serial.write(highByte(_checksum));
    serial.write(lowByte(_checksum));

    return true;
}
bool R30xProtocol::VerifyPassword()
{
    R30xPacket packet;

    packet.SetInstruction(
        R30x::Instruction::VerifyPassword);

    packet.Send(_serial);

    packet.Receive(_serial);

    return packet.ConfirmationCode() ==
           R30x::Confirmation::Success;
}
bool R30xProtocol::GenerateImage()
{
    R30xPacket packet;

    packet.SetInstruction(
        R30x::Instruction::GenImg);

    packet.Send(_serial);

    packet.Receive(_serial);

    return packet.ConfirmationCode() ==
           R30x::Confirmation::Success;
}
bool R30xPacket::Receive(
    HardwareSerial& serial,
    uint32_t timeout)
{
    Clear();

    uint32_t start = millis();

    //-------------------------------
    // Wait for packet header
    //-------------------------------

    while (true)
    {
        if (millis() - start > timeout)
        {
            return false;
        }

        if (serial.available() < 2)
        {
            continue;
        }

        uint16_t header =
            (serial.read() << 8) |
             serial.read();

        if (header == R30x::Header)
        {
            break;
        }
    }

    //-------------------------------
    // Read Address
    //-------------------------------

    while (serial.available() < 4)
    {
        if (millis() - start > timeout)
            return false;
    }

    _address = 0;

    _address |= ((uint32_t)serial.read() << 24);
    _address |= ((uint32_t)serial.read() << 16);
    _address |= ((uint32_t)serial.read() << 8);
    _address |= ((uint32_t)serial.read());

    //-------------------------------
    // Packet Type
    //-------------------------------

    while (serial.available() < 1)
    {
        if (millis() - start > timeout)
            return false;
    }

    _type = static_cast<R30x::PacketType>(
        serial.read());

    //-------------------------------
    // Length
    //-------------------------------

    while (serial.available() < 2)
    {
        if (millis() - start > timeout)
            return false;
    }

    uint16_t length =
        (serial.read() << 8) |
         serial.read();

    if(length < 2)
        return false;

    _payloadLength = length - 2;

    if(_payloadLength > sizeof(_payload))
        return false;

    //-------------------------------
    // Payload
    //-------------------------------

    uint16_t received = 0;

    while(received < _payloadLength)
    {
        if(serial.available())
        {
            _payload[received++] =
                serial.read();
        }

        if(millis() - start > timeout)
            return false;
    }

    //-------------------------------
    // Checksum
    //-------------------------------

    while(serial.available() < 2)
    {
        if(millis() - start > timeout)
            return false;
    }

    _checksum =
        (serial.read() << 8) |
         serial.read();

    //-------------------------------
    // Validate
    //-------------------------------

    _valid =
        (_checksum == CalculateChecksum());

    return _valid;
}
uint32_t R30xPacket::Address() const
{
    return _address;
}
R30x::PacketType R30xPacket::Type() const
{
    return _type;
}
const uint8_t* R30xPacket::Payload() const
{
    return _payload;
}
uint16_t R30xPacket::PayloadLength() const
{
    return _payloadLength;
}
uint8_t R30xPacket::ConfirmationCode() const
{
    if(_payloadLength == 0)
        return 0xFF;

    return _payload[0];
}
bool R30xPacket::IsValid() const
{
    return _valid;
}
bool R30xPacket::IsAckPacket() const
{
    return _type == R30x::PacketType::AckPacket;
}
bool R30xPacket::IsDataPacket() const
{
    return _type == R30x::PacketType::DataPacket;
}
bool R30xPacket::IsEndDataPacket() const
{
    return _type == R30x::PacketType::EndDataPacket;
}