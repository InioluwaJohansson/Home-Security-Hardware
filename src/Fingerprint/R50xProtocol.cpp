#include "R50xProtocol.h"

#include "R50xPacket.h"

R50xProtocol::R50xProtocol(
    HardwareSerial& serial)
    :
    _serial(serial)
{
}

const R50xSystemParameters&
R50xProtocol::GetSystemParameters() const
{
    return _systemParameters;
}
bool R50xProtocol::Initialize()
{
    _serial.begin(
        57600,
        SERIAL_8N1);

    while(_serial.available())
    {
        _serial.read();
    }

    if(!VerifyPassword())
    {
        return false;
    }

    ReadSystemParameters();

    return true;
}
bool R50xProtocol::VerifyPassword()
{
    R50xPacket packet;

    packet.SetInstruction(
        R50x::Instruction::VerifyPassword);

    uint8_t password[4] =
    {
        0x00,
        0x00,
        0x00,
        0x00
    };

    packet.SetPayload(
        password,
        sizeof(password));

    if(!packet.Send(_serial))
    {
        return false;
    }

    if(!packet.Receive(_serial))
    {
        return false;
    }

    if(!packet.IsAckPacket())
    {
        return false;
    }

    return packet.ConfirmationCode() ==
           R50x::Confirmation::Success;
}
bool R50xProtocol::ReadSystemParameters()
{
    R50xPacket packet;

    packet.SetInstruction(
        R50x::Instruction::ReadSysPara);

    if(!packet.Send(_serial))
    {
        return false;
    }

    if(!packet.Receive(_serial))
    {
        return false;
    }

    if(!packet.IsAckPacket())
    {
        return false;
    }

    if(packet.ConfirmationCode() !=
       R50x::Confirmation::Success)
    {
        return false;
    }

    const uint8_t* payload =
        packet.Payload();

    //--------------------------------------------------
    // Payload
    //
    // Byte 0 = Confirmation Code
    // Byte 1-2 = Status Register
    // Byte 3-4 = System Identifier
    // Byte 5-6 = Library Size
    // Byte 7-8 = Security Level
    // Byte 9-12 = Device Address
    // Byte 13-14 = Packet Length Code
    // Byte 15-16 = Baud Rate Code
    //--------------------------------------------------

    _systemParameters.StatusRegister =
        (payload[1] << 8) |
         payload[2];

    _systemParameters.SystemIdentifier =
        (payload[3] << 8) |
         payload[4];

    _systemParameters.LibraryCapacity =
        (payload[5] << 8) |
         payload[6];

    _systemParameters.SecurityLevel =
        (payload[7] << 8) |
         payload[8];

    _systemParameters.DeviceAddress =
        ((uint32_t)payload[9] << 24) |
        ((uint32_t)payload[10] << 16) |
        ((uint32_t)payload[11] << 8) |
         (uint32_t)payload[12];

    //--------------------------------------------------
    // Packet Length
    //--------------------------------------------------

    uint16_t packetLengthCode =
        (payload[13] << 8) |
         payload[14];
    _systemParameters.PacketLengthCode = packetLengthCode;
    switch(packetLengthCode)
    {
        case 0:
            _systemParameters.PacketLength = 32;
            break;

        case 1:
            _systemParameters.PacketLength = 64;
            break;

        case 2:
            _systemParameters.PacketLength = 128;
            break;

        case 3:
            _systemParameters.PacketLength = 256;
            break;

        default:
            _systemParameters.PacketLength = 128;
            break;
    }

    //--------------------------------------------------
    // Baud Rate
    //--------------------------------------------------

    uint16_t baudMultiplier =
        (payload[15] << 8) |
         payload[16];

    _systemParameters.BaudRate =
        baudMultiplier * 9600;

    return true;
}
bool R50xProtocol::GenerateImage()
{
    R50xPacket packet;

    packet.SetInstruction(
        R50x::Instruction::GenImg);

    if(!packet.Send(_serial))
    {
        return false;
    }

    if(!packet.Receive(_serial))
    {
        return false;
    }

    if(!packet.IsAckPacket())
    {
        return false;
    }

    return packet.ConfirmationCode() ==
           R50x::Confirmation::Success;
}
bool R50xProtocol::IsFingerDetected()
{
    R50xPacket packet;

    packet.SetInstruction(
        R50x::Instruction::GenImg);

    if(!packet.Send(_serial))
    {
        return false;
    }

    if(!packet.Receive(_serial))
    {
        return false;
    }

    if(!packet.IsAckPacket())
    {
        return false;
    }

    switch(packet.ConfirmationCode())
    {
        case R50x::Confirmation::Success:
            return true;

        case R50x::Confirmation::NoFinger:
            return false;

        default:
            return false;
    }
}
bool R50xProtocol::UploadImage(
    uint8_t*& image,
    uint32_t& length)
{
    image = nullptr;
    length = 0;

    //---------------------------------------------------
    // Send Upload Image Command
    //---------------------------------------------------

    R50xPacket packet;

    packet.SetInstruction(
        R50x::Instruction::UpImage);

    if(!packet.Send(_serial))
    {
        return false;
    }

    //---------------------------------------------------
    // Wait for ACK
    //---------------------------------------------------

    if(!packet.Receive(_serial))
    {
        return false;
    }

    if(!packet.IsAckPacket())
    {
        return false;
    }

    if(packet.ConfirmationCode() !=
       R50x::Confirmation::Success)
    {
        return false;
    }

    //---------------------------------------------------
    // Receive Image
    //---------------------------------------------------

    if(!ReceiveImagePackets(
            image,
            length))
    {
        return false;
    }

    return true;
}
bool R50xProtocol::ReceiveImagePackets(
    uint8_t*& image,
    uint32_t& imageLength)
{
    if(_imageBuffer != nullptr)
    {
        delete[] _imageBuffer;
        _imageBuffer = nullptr;
    }

    _imageBuffer = new uint8_t[MaxImageSize];

    if(_imageBuffer == nullptr)
    {
        return false;
    }

    imageLength = 0;

    while(true)
    {
        R50xPacket packet;

        if(!packet.Receive(_serial,5000))
        {
            delete[] _imageBuffer;
            _imageBuffer = nullptr;

            return false;
        }

        //------------------------------------------------
        // DATA
        //------------------------------------------------

        if(packet.IsDataPacket())
        {
            uint16_t payloadLength =
                packet.PayloadLength();

            if(imageLength + payloadLength >
               MaxImageSize)
            {
                delete[] _imageBuffer;
                _imageBuffer = nullptr;

                return false;
            }

            memcpy(
                _imageBuffer + imageLength,
                packet.Payload(),
                payloadLength);

            imageLength += payloadLength;

            continue;
        }

        //------------------------------------------------
        // LAST PACKET
        //------------------------------------------------

        if(packet.IsEndDataPacket())
        {
            uint16_t payloadLength =
                packet.PayloadLength();

            if(imageLength + payloadLength >
               MaxImageSize)
            {
                delete[] _imageBuffer;
                _imageBuffer = nullptr;

                return false;
            }

            memcpy(
                _imageBuffer + imageLength,
                packet.Payload(),
                payloadLength);

            imageLength += payloadLength;

            break;
        }

        //------------------------------------------------
        // Invalid Packet
        //------------------------------------------------

        delete[] _imageBuffer;
        _imageBuffer = nullptr;

        return false;
    }

    _imageLength = imageLength;

    image = _imageBuffer;

    return true;
}
bool R50xProtocol::CaptureImage()
{
    return GenerateImage();
}
void R50xProtocol::Cancel()
{
    if(_imageBuffer)
    {
        delete[] _imageBuffer;

        _imageBuffer = nullptr;
    }

    _imageLength = 0;
}