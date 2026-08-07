#pragma once

#include <Arduino.h>

#include "ConfigModel.h"
#include "../Storage/Storage.h"
#include "../Authentication/JWTAuth.h"

class ConfigurationManager
{
public:

    bool Initialize();

    Config& GetConfiguration();

    bool Save();

    void SetJwt(
        const String& token);

    const String& GetJwt() const;

    bool IsAuthenticated() const;

    void Logout();

    bool IsProvisioned() const;

private:

    StorageManager storage;

    JwtManager jwt;

    Config configuration;
};