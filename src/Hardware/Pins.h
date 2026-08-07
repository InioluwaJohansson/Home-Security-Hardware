#pragma once

#include <Arduino.h>
#include <Adafruit_MCP23X17.h>
namespace Pins
{
    //==================================================
    // SPI - MFRC522 RFID
    //==================================================

    constexpr uint8_t RFID_SCK  = 18;
    constexpr uint8_t RFID_MISO = 19;
    constexpr uint8_t RFID_MOSI = 23;
    constexpr uint8_t RFID_SS   = 5;
    constexpr uint8_t RFID_RST  = 4;

    //==================================================
    // UART2 - Fingerprint Scanner
    //==================================================

    constexpr uint8_t FP_TX = 17;
    constexpr uint8_t FP_RX = 16;

    //==================================================
    // I2C Expansion
    //==================================================

    constexpr uint8_t I2C_SDA = 21;
    constexpr uint8_t I2C_SCL = 22;

    //==================================================
    // 74HC4067 Shared Address Pins
    //==================================================

    constexpr uint8_t MUX_S0 = 25;
    constexpr uint8_t MUX_S1 = 26;
    constexpr uint8_t MUX_S2 = 27;
    constexpr uint8_t MUX_S3 = 14;

    //==================================================
    // Multiplexer Enable Pins
    //==================================================

    constexpr uint8_t LIGHT_MUX_EN      = 13;
    constexpr uint8_t APPLIANCE_MUX_EN  = 12;
    constexpr uint8_t DOOR_MUX_EN       = 33;
    constexpr uint8_t WINDOW_MUX_EN     = 32;
    constexpr uint8_t CAMERA_MUX_EN     = 15;
    constexpr uint8_t EXTERNAL_MUX_EN   = 2;

    //==================================================
    // Multiplexer Signal Pins
    //==================================================

    constexpr uint8_t LIGHT_MUX_SIG      = 0;
    constexpr uint8_t APPLIANCE_MUX_SIG  = 34;
    constexpr uint8_t DOOR_MUX_SIG       = 35;
    constexpr uint8_t WINDOW_MUX_SIG     = 36;
    constexpr uint8_t CAMERA_MUX_SIG     = 39;
    constexpr uint8_t EXTERNAL_MUX_SIG   = 1;

    constexpr uint8_t MUX_SIG   = 1;

    //==================================================
    // Alarm
    //==================================================

    constexpr uint8_t BUZZER = 3;

    //==================================================
    // Reset Button
    //==================================================

    constexpr uint8_t RESET_BUTTON = 10;
}
class MCP23017Controller
{
public:

    bool Initialize();

    void DigitalWrite(
        uint8_t chip,
        uint8_t pin,
        bool state);

    bool DigitalRead(
        uint8_t chip,
        uint8_t pin);

private:

    Adafruit_MCP23X17 _chips[3];
};

class MultiplexerController
{
public:

    void Initialize();

    void SelectChannel(uint8_t channel);

    bool ReadChannel(uint8_t channel);
    int AnalogRead(uint8_t channel);

private:

    void SetAddress(uint8_t channel);
};
namespace HardwareOutputs
{
    constexpr uint8_t AlarmChip = 2;   // Third MCP23017

    constexpr uint8_t BuzzerPin = 8;   // GPB0

    constexpr uint8_t SirenPin  = 9;   // GPB1
}