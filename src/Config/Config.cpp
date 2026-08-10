#include "Config.h"

bool ConfigurationManager::Initialize()
{
    if (!storage.Begin())
        return false;

    configuration = storage.LoadConfiguration();
    return true;
}

Config& ConfigurationManager::GetConfiguration()
{
    return configuration;
}

bool ConfigurationManager::Save()
{
    return storage.SaveConfiguration(configuration);
}

void ConfigurationManager::SetJwt(const String& token)
{
    jwt.SetToken(token);
}

const String& ConfigurationManager::GetJwt() const
{
    return jwt.GetToken();
}

bool ConfigurationManager::IsAuthenticated() const
{
    return jwt.HasToken();
}

void ConfigurationManager::Logout()
{
    jwt.Clear();
}

bool ConfigurationManager::IsProvisioned() const
{
    return configuration.IsProvisioned;
}

bool ConfigurationManager::ResetPreferences()
{
    jwt.Clear();
    storage.Clear();
    configuration = Config{};
    return true;
}
