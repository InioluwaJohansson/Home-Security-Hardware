#pragma once

#include <Arduino.h>

class MultiplexerController
{
public:
    enum class Bus : uint8_t { Door = 0, Window = 1, External = 2 };

    void Initialize();
    bool DigitalRead(Bus bus, uint8_t channel, bool activeHigh = true);
    int AnalogRead(Bus bus, uint8_t channel);

private:
    void SetAddress(uint8_t channel);
    int SignalPin(Bus bus) const;
};
