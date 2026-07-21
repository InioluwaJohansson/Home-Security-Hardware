#include "Config.h"

bool ConfigurationManager::Initialize()
{
    storage.Begin();
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
    return jwt.IsAuthenticated();
}
