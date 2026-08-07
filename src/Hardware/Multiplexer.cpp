#include "Multiplexer.h"

#include "Pins.h"

void MultiplexerController::Initialize()
{
    pinMode(Pins::MUX_S0, OUTPUT);
    pinMode(Pins::MUX_S1, OUTPUT);
    pinMode(Pins::MUX_S2, OUTPUT);
    pinMode(Pins::MUX_S3, OUTPUT);

    pinMode(Pins::MUX_SIG, INPUT);

    digitalWrite(Pins::MUX_S0, LOW);
    digitalWrite(Pins::MUX_S1, LOW);
    digitalWrite(Pins::MUX_S2, LOW);
    digitalWrite(Pins::MUX_S3, LOW);
}

void MultiplexerController::SelectChannel(uint8_t channel)
{
    if(channel > 15)
        return;

    SetAddress(channel);

    delayMicroseconds(5);
}

void MultiplexerController::SetAddress(uint8_t channel)
{
    digitalWrite(Pins::MUX_S0, bitRead(channel, 0));
    digitalWrite(Pins::MUX_S1, bitRead(channel, 1));
    digitalWrite(Pins::MUX_S2, bitRead(channel, 2));
    digitalWrite(Pins::MUX_S3, bitRead(channel, 3));
}

bool MultiplexerController::ReadChannel(uint8_t channel)
{
    SelectChannel(channel);

    return digitalRead(Pins::MUX_SIG);
}

int MultiplexerController::AnalogRead(uint8_t channel)
{
    SelectChannel(channel);

    return analogRead(Pins::MUX_SIG);
}