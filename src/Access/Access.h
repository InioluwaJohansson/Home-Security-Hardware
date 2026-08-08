#pragma once

#include "../Facilities/Door.h"

class Access
{
public:
    explicit Access(DoorManager& door);

    bool Initialize();

    void Grant(int doorId = 0);
    void Deny();
    void Update();

private:
    DoorManager& _door;
};
