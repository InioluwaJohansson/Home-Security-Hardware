#pragma once

#include <Arduino.h>
#include <HTTPClient.h>

#include "../Config/Config.h"
#include <HTTPClient.h>
class AuthenticationManager
{
public:

    AuthenticationManager(ConfigurationManager& configuration);

    bool Authenticate();

private:

    ConfigurationManager& _configuration;
};