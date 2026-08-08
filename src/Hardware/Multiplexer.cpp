#include "Multiplexer.h"
#include "Pins.h"

void MultiplexerController::Initialize()
{
    pinMode(Pins::MUX_S0, OUTPUT);
    pinMode(Pins::MUX_S1, OUTPUT);
    pinMode(Pins::MUX_S2, OUTPUT);
    pinMode(Pins::MUX_S3, OUTPUT);
    pinMode(Pins::MUX_DOOR_SIG, INPUT);
    pinMode(Pins::MUX_WINDOW_SIG, INPUT);
    pinMode(Pins::MUX_EXTERNAL_SIG, INPUT);

    digitalWrite(Pins::MUX_S0, LOW);
    digitalWrite(Pins::MUX_S1, LOW);
    digitalWrite(Pins::MUX_S2, LOW);
    digitalWrite(Pins::MUX_S3, LOW);
}

int MultiplexerController::SignalPin(Bus bus) const
{
    switch (bus)
    {
        case Bus::Door: return Pins::MUX_DOOR_SIG;
        case Bus::Window: return Pins::MUX_WINDOW_SIG;
        case Bus::External: return Pins::MUX_EXTERNAL_SIG;
        default: return -1;
    }
}

void MultiplexerController::SetAddress(uint8_t channel)
{
    digitalWrite(Pins::MUX_S0, bitRead(channel, 0));
    digitalWrite(Pins::MUX_S1, bitRead(channel, 1));
    digitalWrite(Pins::MUX_S2, bitRead(channel, 2));
    digitalWrite(Pins::MUX_S3, bitRead(channel, 3));
}

bool MultiplexerController::DigitalRead(Bus bus, uint8_t channel, bool activeHigh)
{
    if (channel >= 16) return false;
    const int signal = SignalPin(bus);
    if (signal < 0) return false;
    SetAddress(channel);
    delayMicroseconds(5);
    const bool value = ::digitalRead(signal) == HIGH;
    return activeHigh ? value : !value;
}

int MultiplexerController::AnalogRead(Bus bus, uint8_t channel)
{
    if (channel >= 16) return 0;
    const int signal = SignalPin(bus);
    if (signal < 0) return 0;
    SetAddress(channel);
    delayMicroseconds(5);
    return ::analogRead(signal);
}
