#pragma once

#include <Arduino.h>

class JwtManager
{
public:
    void SetToken(const String& token);
    String GetToken() const;
    bool HasToken() const;
    void Clear();

private:

    String _token;
};