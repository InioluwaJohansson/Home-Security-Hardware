#pragma once

#include <Arduino.h>

enum class FingerprintOperation
{
    None,

    Registration,

    Verification
};

enum class FingerprintState
{
    Idle,

    WaitingForFinger,

    Capturing,

    Uploading,

    Publishing,

    Success,

    Failed
};

struct FingerprintContext
{
    FingerprintOperation Operation =
        FingerprintOperation::None;

    FingerprintState State =
        FingerprintState::Idle;

    int PersonId = -1;

    unsigned long StartedAt = 0;
};