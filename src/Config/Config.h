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

    // Clears all persisted provisioning/preferences and JWT state.
    bool ResetPreferences();

private:

    StorageManager storage;

    JwtManager jwt;

    Config configuration;
};