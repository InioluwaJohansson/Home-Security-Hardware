#pragma once
#include <Arduino.h>
#include "../Config/Config.h"

class StorageManager
{
public:

    bool Begin();

    bool SaveConfiguration(const DeviceConfiguration& configuration);

    DeviceConfiguration LoadConfiguration();

    bool IsProvisioned();

    void Clear();

private:

    const char* Namespace = "HomeSecurity";
};