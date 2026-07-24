#pragma once

#include <Arduino.h>

class JwtManager
{
public:

    void SetToken(const String& token);

    const String& GetToken() const;

    bool HasToken() const;

    void Clear();

private:

    String _token;
};