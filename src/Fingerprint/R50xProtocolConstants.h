#pragma once

#include <Arduino.h>

namespace R50x
{
    //--------------------------------------------------
    // Protocol Header
    //--------------------------------------------------

    constexpr uint16_t Header = 0xEF01;

    constexpr uint32_t Address = 0xFFFFFFFF;

    //--------------------------------------------------
    // Packet Types
    //--------------------------------------------------

    enum PacketType : uint8_t
    {
        CommandPacket = 0x01,

        DataPacket = 0x02,

        AckPacket = 0x07,

        EndDataPacket = 0x08
    };

    //--------------------------------------------------
    // Instructions
    //--------------------------------------------------

    enum Instruction : uint8_t
    {
        GenImg          = 0x01,

        Img2Tz          = 0x02,

        Match           = 0x03,

        Search          = 0x04,

        RegModel        = 0x05,

        Store           = 0x06,

        LoadChar        = 0x07,

        UpChar          = 0x08,

        DownChar        = 0x09,

        UpImage         = 0x0A,

        DownImage       = 0x0B,

        DeleteChar      = 0x0C,

        Empty           = 0x0D,

        SetSysPara      = 0x0E,

        ReadSysPara     = 0x0F,

        SetPassword     = 0x12,

        VerifyPassword  = 0x13,

        GetRandomCode   = 0x14,

        SetAddress      = 0x15,

        PortControl     = 0x17,

        WriteNotepad    = 0x18,

        ReadNotepad     = 0x19,

        HighSpeedSearch = 0x1B,

        TemplateNum     = 0x1D
    };

    //--------------------------------------------------
    // Confirmation Codes
    //--------------------------------------------------

    enum Confirmation : uint8_t
    {
        Success                 = 0x00,

        PacketReceiveError      = 0x01,

        NoFinger                = 0x02,

        ImageFail               = 0x03,

        ImageMessy              = 0x06,

        FeatureFail             = 0x07,

        NoMatch                 = 0x09,

        NotFound                = 0x09,

        MergeFail               = 0x0A,

        AddressError            = 0x20,

        PasswordError           = 0x13
    };
}