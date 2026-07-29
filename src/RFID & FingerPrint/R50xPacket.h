#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

#include "R50xProtocolConstants.h"

class R30xPacket
{
public:

    R30xPacket();

    void Clear();

    //------------------------------------------------------
    // Build Packet
    //------------------------------------------------------

    void SetAddress(uint32_t address);

    void SetPacketType(R30x::PacketType type);

    void SetPayload(
        const uint8_t* data,
        uint16_t length);

    void SetInstruction(
        R30x::Instruction instruction);

    //------------------------------------------------------
    // Serialize
    //------------------------------------------------------

    bool Send(
        HardwareSerial& serial);

    //------------------------------------------------------
    // Deserialize
    //------------------------------------------------------

    bool Receive(
        HardwareSerial& serial,
        uint32_t timeout = 3000);

    //------------------------------------------------------
    // Accessors
    //------------------------------------------------------

    bool IsAckPacket() const;

bool IsDataPacket() const;

bool IsEndDataPacket() const;

    uint32_t Address() const;

    R30x::PacketType Type() const;

    const uint8_t* Payload() const;

    uint16_t PayloadLength() const;

    uint8_t ConfirmationCode() const;

    bool IsValid() const;

private:

    uint16_t CalculateChecksum() const;

private:

    uint32_t _address;

    R30x::PacketType _type;

    uint8_t _payload[512];

    uint16_t _payloadLength;

    uint16_t _checksum;

    bool _valid;
};