#pragma once

#include <Arduino.h>

enum class NFIDOperation
{
    None,

    Read,

    Write
};

enum class NFIDState
{
    Idle,

    WaitingForCard,

    Reading,

    Writing,

    Publishing,

    Success,

    Failed
};

struct NFIDContext
{
    NFIDOperation Operation = NFIDOperation::None;

    NFIDState State = NFIDState::Idle;

    String NFIDCode;

    unsigned long StartedAt = 0;
};

struct NFIDVerifyDto
{
    String Token;
    String HardwareId;
    String NFIDCode;
};