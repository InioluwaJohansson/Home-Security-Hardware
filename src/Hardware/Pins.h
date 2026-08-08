#pragma once

#include <Arduino.h>

namespace Pins
{
    //==================================================
    // UART2 - R50x fingerprint sensor
    //==================================================
    constexpr uint8_t FP_TX = 17;
    constexpr uint8_t FP_RX = 16;

    //==================================================
    // I2C - MCP23017 x4 / PCA9685 x2 / ADS1115 x4 / PN532 NFID
    //==================================================
    constexpr uint8_t I2C_SDA = 21;
    constexpr uint8_t I2C_SCL = 22;

    //==================================================
    // I2C PN532 NFID control pins
    // Adafruit PN532 requires dedicated IRQ and RESET pins
    // for its I2C constructor.
    //==================================================
    constexpr uint8_t PN532_IRQ = 32;
    constexpr uint8_t PN532_RESET = 33;

    //==================================================
    // 74HC4067 shared address bus
    // Three multiplexers share S0-S3 and have separate SIG inputs.
    //==================================================
    constexpr uint8_t MUX_S0 = 25;
    constexpr uint8_t MUX_S1 = 26;
    constexpr uint8_t MUX_S2 = 27;
    constexpr uint8_t MUX_S3 = 14;

    constexpr uint8_t MUX_DOOR_SIG = 34;
    constexpr uint8_t MUX_WINDOW_SIG = 35;
    constexpr uint8_t MUX_EXTERNAL_SIG = 36;

    static_assert(MUX_DOOR_SIG >= 34, "Door MUX SIG must be input-only GPIO");
    static_assert(MUX_WINDOW_SIG >= 34, "Window MUX SIG must be input-only GPIO");
    static_assert(MUX_EXTERNAL_SIG >= 34, "External MUX SIG must be input-only GPIO");

    //==================================================
    // Local control
    //==================================================
    constexpr uint8_t RESET_BUTTON = 10;
}

namespace HardwareOutputs
{
    constexpr uint8_t AlarmChip = 2;
    constexpr uint8_t BuzzerPin = 13;
    constexpr uint8_t SirenPin = 14;
}
