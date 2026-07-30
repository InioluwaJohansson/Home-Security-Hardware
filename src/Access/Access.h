#pragma once

#include "../Facilities/Door.h"

class Access
{
public:

    Access(DoorManager& door);

    void Grant(int doorId = 0);

    void Deny();

private:

    DoorManager& _door;
};