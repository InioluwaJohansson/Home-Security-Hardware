#pragma once

#include <Arduino.h>

class MultiplexerController
{
public:

    void Initialize();

    void SelectChannel(uint8_t channel);

    bool DigitalRead(uint8_t channel);

    int AnalogRead(uint8_t channel);

private:

    void SetAddress(uint8_t channel);
};