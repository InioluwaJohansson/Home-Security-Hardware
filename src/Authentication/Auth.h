#pragma once

#include <Arduino.h>
#include "../Config/Config.h"

class AuthenticationManager
{
public:
    explicit AuthenticationManager(
        ConfigurationManager& configuration);

    bool Authenticate();

private:
    ConfigurationManager& _configuration;
};
