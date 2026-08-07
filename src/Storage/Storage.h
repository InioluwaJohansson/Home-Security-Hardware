#pragma once
#include <Arduino.h>
#include "../Config/ConfigModel.h"

class StorageManager
{
public:

    bool Begin();

    bool SaveConfiguration(const Config& configuration);

    Config LoadConfiguration();

    bool IsProvisioned();

    void Clear();

private:

    const char* Namespace = "HomeSecurity";
};